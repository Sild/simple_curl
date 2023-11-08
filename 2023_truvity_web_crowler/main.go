package main

import (
	"crawler/crawler"
	"log"
	"os"
	"os/signal"
	"strconv"
	"syscall"
)

func main() {
	log.Println("App started...")
	defer log.Println("App finished")

	if len(os.Args) < 3 {
		log.Fatalln("Usage: ./app url dst_path {threads}")
	}

	url := os.Args[1]
	dstFolder := os.Args[2]
	workersCount := 1
	if len(os.Args) > 3 {
		threads, err := strconv.Atoi(os.Args[3])
		if err != nil {
			log.Fatal(err)
		}
		workersCount = threads
	}

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)

	crawler := crawler.New(url, dstFolder)
	go func() {
        <-sig
        log.Println("calling crawler.shutdown()...")
		crawler.Shutdown();
    }()
	crawler.Run(workersCount);
}