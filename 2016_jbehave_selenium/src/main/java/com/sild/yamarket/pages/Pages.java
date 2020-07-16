package com.sild.yamarket.pages;

import org.openqa.selenium.JavascriptExecutor;
import org.openqa.selenium.WebDriver;

public class Pages {
    private WebDriver driver;
    private YandexPage defaultPage;
    private MarketCatalogPage marketCatalogPage;
    private MarketProductPage marketProductPage;
    private MarketSearchBlock marketSearchBlock;

    private static long MAX_AJAX_EXECUTION_TIME = 20000;
    private static long CHECK_AJAX_EXECUTED_PERIOD = 1000;

    public Pages(WebDriver driver) {
        this.driver = driver;
        this.defaultPage = new YandexPage(driver);
        this.marketCatalogPage = new MarketCatalogPage(driver);
        this.marketProductPage = new MarketProductPage(driver);
        this.marketSearchBlock = new MarketSearchBlock(driver);
    }

    public YandexPage defaultPage() {
        return defaultPage;
    }

    public MarketCatalogPage getMarketCatalogPage() {
        return marketCatalogPage;
    }

    public MarketSearchBlock getMarketSearchBlock() {
        return marketSearchBlock;
    }

    public MarketProductPage getMarketProductPage() {
        return marketProductPage;
    }

    public void waitAjaxDone() {
        try {
            Long startWait = System.currentTimeMillis();
            while (System.currentTimeMillis() - startWait < MAX_AJAX_EXECUTION_TIME) {
                Thread.sleep(CHECK_AJAX_EXECUTED_PERIOD);
                Boolean ajaxIsComplete = (Boolean) ((JavascriptExecutor) driver).executeScript("return jQuery.active == 0");
                if (ajaxIsComplete) {
                    break;
                }
            }
        } catch (InterruptedException e) {
            System.out.println("Waiting of ajax response was interrupted");
        }

    }
}
