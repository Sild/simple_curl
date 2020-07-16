package com.sild.yamarket.pages;

import org.junit.Assert;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;

import java.util.List;

public class MarketSearchBlock {
    private final String MIN_PRICE_FIELD_XPATH = "//*[@id=\"gf-pricefrom-var\"]";
    private final String PROVIDER_FILTER_XPATH_TEMPLATE = "//div[@class=\"filter-block__body\"]//label[text() = \"[[PROVIDER_NAME]]\" ]";
    private final String APPLY_SEARCH_XPATH = "//span[text()='Применить']";



    private WebDriver driver;
	public MarketSearchBlock(WebDriver driver) {
        this.driver = driver;
	}

    public void setMinPrice(String minPrice) {
        List<WebElement> elementList = driver.findElements(By.xpath(MIN_PRICE_FIELD_XPATH));
        Assert.assertTrue(elementList.size() > 0);
        elementList.get(0).sendKeys(minPrice);
    }

    public void selectProvider(String providerName) {
        List<WebElement> elementList = driver.findElements(By.xpath(PROVIDER_FILTER_XPATH_TEMPLATE.replace("[[PROVIDER_NAME]]", providerName)));
        Assert.assertTrue(elementList.size() > 0);
        elementList.get(0).click();
    }

    public void applySearch() {
        List<WebElement> elementList = driver.findElements(By.xpath(APPLY_SEARCH_XPATH));
        Assert.assertTrue(elementList.size() > 0);
        elementList.get(0).click();
    }
}
