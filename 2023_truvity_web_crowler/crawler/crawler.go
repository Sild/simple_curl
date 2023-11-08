package crawler

import (
	"bufio"
	"log"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

type Crawler struct {
	queue         chan string
	wg            *sync.WaitGroup
	baseUrl       string          // hasn't ending '/'
	dstRootFolder string          // hasn't ending '/'
	visited       map[string]bool // store full url (including https:...)
	visitedMtx    sync.Mutex
	idleWorkers   atomic.Int32
	workersCount  int
	shutdownMtx   sync.Mutex
	shutdown      bool
}

func New(baseUrl, dstRootFolder string) *Crawler {
	return &Crawler{
		queue:         make(chan string, 10000), // may stuck if overloaded (workers stuck on push). No time to fix.
		wg:            &sync.WaitGroup{},
		baseUrl:       strings.TrimSuffix(baseUrl, "/"),
		dstRootFolder: strings.TrimSuffix(dstRootFolder, "/"),
		visited:       map[string]bool{},
		visitedMtx:    sync.Mutex{},
		idleWorkers:   atomic.Int32{},
		workersCount:  0,
		shutdownMtx:   sync.Mutex{},
		shutdown:      false,
	}
}

func (c *Crawler) Shutdown() {
	c.shutdownMtx.Lock()
	defer c.shutdownMtx.Unlock()
	c.shutdown = true
}

func (c *Crawler) Run(workersCount int) {
	c.workersCount = workersCount
	c.idleWorkers.Store(int32(c.workersCount))

	if _, err := os.Stat(c.dstRootFolder); !os.IsNotExist(err) {
		log.Printf("Directory %s already exists. Do you want to proceed? [y/n]", c.dstRootFolder)
		reader := bufio.NewReader(os.Stdin)
		text, _ := reader.ReadString('\n') // TODO it's waiting for input after ctrl+c. Looks bad.
		if text != "y\n" {
			log.Println("Then I'm done.")
			return
		}
	} else {
		if err := os.Mkdir(c.dstRootFolder, os.ModePerm); err != nil {
			log.Fatal(err)
		}
	}

	// init state - push new urls to queue, workers start doing something
	if err := c.initState(); err != nil {
		log.Fatalf("Error during restoring the old state: %s\n", err.Error())
	}

	c.wg.Add(c.workersCount)
	for i := 0; i < c.workersCount; i++ {
		go c.worker(i + 1)
	}

	// run checker which will wait for idle workers
	c.wg.Add(1)
	go c.checkDone()

	c.wg.Wait()
	close(c.queue)
}

func (c *Crawler) checkDone() {
	defer c.wg.Done()
	// if all workers are waiting on queue - means there is nothing to parse anymore
	ticker := time.NewTicker(500 * time.Millisecond)
	for range ticker.C {
		if int(c.idleWorkers.Load()) == c.workersCount {
			// c.Shutdown()
			break
		}

	}
}

func (c *Crawler) initState() error {
	rootRequired := true
	err := filepath.Walk(c.dstRootFolder,
		func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			if info.IsDir() {
				return nil
			}
			rootRequired = false // we found something in folder, root is not required anymore
			visitedUrl := pathToUrl(path, c.dstRootFolder, c.baseUrl)
			// we have only 1 thread here
			c.visited[visitedUrl] = true
			log.Printf("found in visited: %s\n", visitedUrl)
			data, err := os.ReadFile(path)
			if err != nil {
				return err
			}
			for url := range extractChildrenLinks(string(data), visitedUrl, c.baseUrl) {
				c.queue <- url
			}
			return nil
		})
	if err != nil {
		return err
	}
	if rootRequired {
		c.queue <- c.baseUrl
	}
	return nil
}

func (c *Crawler) worker(workerNum int) {
	log.Printf("worker %d started", workerNum)
	defer func() {
		log.Printf("worker %d finished", workerNum)
		c.wg.Done()
	}()

	shutdown := false
	for !shutdown {
		url := <-c.queue
		c.workerLoop(workerNum, url)
		c.shutdownMtx.Lock()
		shutdown = c.shutdown
		c.shutdownMtx.Unlock()
	}
}

func (c *Crawler) workerLoop(workerNum int, url string) {
	c.idleWorkers.Add(-1)
	defer c.idleWorkers.Add(1)
	visited := false
	c.visitedMtx.Lock()
	if _, visited = c.visited[url]; !visited {
		c.visited[url] = true
	}
	c.visitedMtx.Unlock()
	if visited {
		return
	}
	content, _, err := getPage(url)
	if err != nil {
		log.Printf("fail to get content for url=%s: %s", url, err.Error())
		return
	}

	// TODO handle non-200 code somehow?

	if err := saveContent(url, content, c.dstRootFolder, c.baseUrl); err != nil {
		log.Printf("fail to save file for url=%s: %s", url, err.Error())
		return
	}
	childrenLinks := extractChildrenLinks(string(content), url, c.baseUrl)
	log.Printf("worker %d found new links in %s: %v", workerNum, url, childrenLinks)
	for url, _ := range childrenLinks {
		c.visitedMtx.Lock()
		if _, visited = c.visited[url]; !visited {
			c.queue <- url
		}
		c.visitedMtx.Unlock()
	}
}
