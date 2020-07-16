package com.sild.yamarket.pages;

import org.junit.Assert;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;

import java.util.ArrayList;
import java.util.List;

public class MarketCatalogPage {
    private final String ELECTRONIC_LINK_XPATH = "//a[text() = 'Электроника']";
    private final String TV_LINK_XPATH_TPL = "//a[text() = '[[SUBSECTION_NAME]]']";
    private final String EXTENDED_SEARCH_XPATH = "//a[starts-with(normalize-space(text()),'Расширенный поиск')]";
    private final String RESULT_ITEM_XPATH = "//div[contains(@class, \"filter-applied-results\")]//div[contains(@class, \"snippet-card \")]//span[contains(@class, \"snippet-card__header-text\")]";
    private final String FIND_LINK_XPATH = "//form//button";


    private WebDriver driver;
	public MarketCatalogPage(WebDriver driver) {
        this.driver = driver;
	}

    public void assertTitleStartWith(String title) {
        Assert.assertTrue(driver.getTitle().startsWith(title));
    }

    public void selectElectronicSection() {
        List<WebElement> links = driver.findElements(By.xpath(ELECTRONIC_LINK_XPATH));
        Assert.assertTrue(links.size() > 0);
        links.get(0).click();
    }

    public void selectSubsection(String subsectionName) {
        final String TV_LINK_XPATH = TV_LINK_XPATH_TPL.replace("[[SUBSECTION_NAME]]", subsectionName);
        System.out.println(TV_LINK_XPATH);
        List<WebElement> links = driver.findElements(By.xpath(TV_LINK_XPATH));
        Assert.assertTrue("Fail to find any element with specified xpath", links.size() > 0);
        for(WebElement e: links) {
            if(e.isDisplayed()) {
                e.click();
                return;
            }
        }
    }

    public void activateExtendedSearch() {
        List<WebElement> links = driver.findElements(By.xpath(EXTENDED_SEARCH_XPATH));
        Assert.assertTrue(links.size() > 0);
        links.get(0).click();
    }

    public int getResultCount() {
        List elementList = driver.findElements(By.xpath(RESULT_ITEM_XPATH));
        return elementList.size();
    }

    public List<String> getResultItemNames() {
        List<WebElement> elementList = driver.findElements(By.xpath(RESULT_ITEM_XPATH));
        List<String> itemNames = new ArrayList<>();
        for(WebElement e: elementList) {
            itemNames.add(e.getText());
        }
        return itemNames;
    }

    public void setSearchPanelText(String text) {
        driver.findElement(By.id("header-search")).sendKeys(text);
    }

    public void pressSearchButton() {
        driver.findElement(By.xpath(FIND_LINK_XPATH)).submit();
    }

}
