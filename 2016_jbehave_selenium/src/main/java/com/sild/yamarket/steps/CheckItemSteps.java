package com.sild.yamarket.steps;

import com.sild.yamarket.pages.Pages;
import junit.framework.Assert;
import org.jbehave.core.annotations.*;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.firefox.FirefoxDriver;

import java.util.Arrays;
import java.util.List;

public class CheckItemSteps {
    private WebDriver driver;
    private Pages pages;
    private String storedElementName;

    public CheckItemSteps() {
    }

    @BeforeStory
    public void createBrowser() {
        driver = new FirefoxDriver();
    }

    @AfterStory
    public void closeBrowser() {
        driver.close();
    }

    @Given("browser is open and maximized")
    public void prepareBrowser() {
        org.junit.Assert.assertNotNull(driver);
        driver.manage().window().maximize();
        pages = new Pages(driver);
    }

    @When("navigate to $site")
    public void navigateToSite(String site) {
        driver.navigate().to(site);
    }

    @Then("page title is $title")
    public void assertDefaultTitle(String title) {
        pages.defaultPage().checkTitleEquals(title);
    }

    @When("I click to Market link")
    public void clickMarketLink() {
        pages.defaultPage().clickMarketLink();
    }

    @Then("page title start with $titleStart")
    public void assertMarketTitle(String titleStart) {
        Assert.assertTrue(driver.getTitle().startsWith(titleStart));
    }

    @When("I select Electronic-><subsection>->Extended Search section, set min price to <price>, select <providers> as providers and click Apply button")
    public void specifySearch(@Named("subsection") String subsectionName, @Named("price") String minPrice, @Named("providers") String providers) {
        pages.getMarketCatalogPage().selectElectronicSection();
        pages.getMarketCatalogPage().selectSubsection(subsectionName);
        pages.getMarketCatalogPage().activateExtendedSearch();
        pages.getMarketSearchBlock().setMinPrice(minPrice);
        List<String> providerList = Arrays.asList(providers.split(","));
        for(String provider: providerList) {
            pages.getMarketSearchBlock().selectProvider(provider);
            pages.waitAjaxDone();
        }
        pages.getMarketSearchBlock().applySearch();
        pages.waitAjaxDone();
    }

    @Then("there are $itemCount elements in result")
    public void checkResultCount(String itemCount) {
        Assert.assertEquals(Integer.parseInt(itemCount), pages.getMarketCatalogPage().getResultCount());
    }

    @When("I remember the item №$itemNumToRemember, set its name to search panel and press search button")
    public void storeResult(String itemNumToRemember) {
        storedElementName = pages.getMarketCatalogPage().getResultItemNames().get(Integer.parseInt(itemNumToRemember) - 1);
        pages.getMarketCatalogPage().setSearchPanelText(storedElementName);
        pages.getMarketCatalogPage().pressSearchButton();
    }

    @Then("result equals to first item")
    public void checkResultEqualsToStored() {
        Assert.assertEquals(storedElementName, pages.getMarketProductPage().getItemName());
    }
}
