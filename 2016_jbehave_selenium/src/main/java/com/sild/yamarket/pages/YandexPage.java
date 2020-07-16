package com.sild.yamarket.pages;

import junit.framework.Assert;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;

import java.util.List;

public class YandexPage {
    private final String MARKET_LINK_XPATH = "//a[text() = 'Маркет']";


    private WebDriver driver;
	YandexPage(WebDriver driver) {
        this.driver = driver;
	}

	public void checkTitleEquals(String title) {
        Assert.assertEquals(title, driver.getTitle());
    }

    public void clickMarketLink() {
        List<WebElement> marketLinks = driver.findElements(By.xpath(MARKET_LINK_XPATH));
        Assert.assertTrue(marketLinks.size() > 0);
        marketLinks.get(0).click();
    }
}
