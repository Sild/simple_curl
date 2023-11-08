package crawler

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestExtractLinks(t *testing.T) {
	content := "bla bla <a href=\"123\">sometext"
	expected := map[string]bool{"123": true}
	given := extractLinks(content)
	assert.Equal(t, expected, given)

	content = "bla bla <a href=\"123\"___ttt>sometextbla bla <a href='234''''ccc>sometext"
	expected = map[string]bool{"123": true, "234": true}
	given = extractLinks(content)
	assert.Equal(t, expected, given)
}

func TestPathToUrl(t *testing.T) {
	baseUrl := "https://google.com"
	rootFolder := "/tmp/google"
	path := "/tmp/google/123.html"
	expected := "https://google.com/123"
	given := pathToUrl(path, rootFolder, baseUrl)
	assert.Equal(t, expected, given)

	path = "/tmp/google/index.html"
	expected = "https://google.com"
	given = pathToUrl(path, rootFolder, baseUrl)
	assert.Equal(t, expected, given)
}