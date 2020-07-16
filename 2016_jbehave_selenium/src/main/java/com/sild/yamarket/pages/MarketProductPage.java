package com.sild.yamarket.pages;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;


public class MarketProductPage {
    private final String ITEM_NAME_HEADER_XPATH = "//div[contains(@class, \"headline__header\")]/h1";
    private WebDriver driver;

	public MarketProductPage(WebDriver driver) {
        this.driver = driver;
	}

    public String getItemName() {
        return driver.findElement(By.xpath(ITEM_NAME_HEADER_XPATH)).getText();
    }

    
}
