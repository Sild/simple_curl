package crawler

import (
	"io"
	"log"
	"net/http"
	"os"
	"path"
	"regexp"
	"strings"
)

// return content, status, error
func getPage(url string) (string, int, error) {
	res, err := http.Get(url)
    if err != nil {
		return "", 0, err
    }

	// TODO should I handle non-200 codes somehow? redirects for example?
	
    content, err := io.ReadAll(res.Body)
    res.Body.Close()
    if err != nil {
       return "", res.StatusCode, err
    }
    return string(content), res.StatusCode, nil
}

func extractChildrenLinks(content string, parentUrl string, baseUrl string) map[string]bool {
	allLinks := extractLinks(content)
	// TODO what should I do if page contains references to the same deep level?
	// like http://test.com/123 -> http://test.com/234 ?
	// Sites are complicated. That's why it has robots.txt.
	// I assume all out paths has format like /test and nothing else
	
	filtered := map[string]bool{}
	for link := range allLinks {
		fullLink := link
		if !strings.HasPrefix(fullLink, "http") {
			if strings.HasPrefix(fullLink, "/") {
				fullLink = baseUrl + fullLink
			} else {
				fullLink = parentUrl + fullLink
			}
		}
		// log.Printf("full link: %s", fullLink)
		if strings.HasPrefix(fullLink, parentUrl) {
			filtered[fullLink] = true
		}
	}
	return filtered
}

func extractLinks(content string) map[string]bool {
	re := regexp.MustCompile(`href=["'](.*?)["']`)
	matches := re.FindAllStringSubmatch(content, -1)

	links := map[string]bool{}
	for _, match := range matches {
		lnk := match[1]
		// cut everything after '?'
		questionPos := strings.Index(lnk, "?")
		if questionPos != -1 {
			lnk = lnk[0: questionPos]
		}
		anotherPos := strings.Index(lnk, "#")
		if anotherPos != -1 {
			lnk = lnk[0: anotherPos]
		}
		links[lnk] = true
	}

	return links
}

func pathToUrl(path string, rootFolder string, baseUrl string) string {
	path = strings.TrimPrefix(path, rootFolder)
	path = strings.TrimSuffix(path, ".html")
	path = strings.TrimSuffix(path, "/index")
	return baseUrl + path
}

func urlToPath(url string, rootFolder string, baseUrl string) string {
	url = strings.TrimSuffix(url, "/")
	url = strings.TrimPrefix(url, baseUrl)
	if url == "" {
		url = "index"
	}
	return rootFolder + "/" + url + ".html"
}

func saveContent(url string, content string, rootFolder string, baseUrl string) error {
	// not sure if I should use mutex for file/directory creation
	filePath := urlToPath(url, rootFolder, baseUrl)
	dirPath := path.Dir(filePath)
	if _, err := os.Stat(dirPath); os.IsNotExist(err) { 
		if err := os.MkdirAll(dirPath, 0700); err != nil {
			return nil
		}
	}
	f, err := os.Create(filePath)
	if err != nil {
		return err
	}
	defer f.Close()

	_, err = f.WriteString(content)
	if err != nil {
		return err
	}
	log.Printf("saved %s into %s\n", url, filePath)
	return nil
}