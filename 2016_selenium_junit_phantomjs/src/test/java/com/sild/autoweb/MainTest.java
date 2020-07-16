package com.sild.autoweb;

/**
 * @author Dmitry Korchagin.
 */

import org.apache.commons.io.FileUtils;
import org.junit.*;
import org.junit.rules.TestName;
import org.openqa.selenium.Capabilities;
import org.openqa.selenium.OutputType;
import org.openqa.selenium.TakesScreenshot;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.phantomjs.PhantomJSDriver;
import org.openqa.selenium.phantomjs.PhantomJSDriverService;
import org.openqa.selenium.remote.DesiredCapabilities;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNotNull;

public class MainTest {
    private static final Logger logger = LoggerFactory.getLogger(MainTest.class);
    private WebDriver driver;
    private static DesiredCapabilities capabilities;

    @Rule
    public TestName testName = new TestName();

    @BeforeClass
    public static void setUp() {
        try {
            FileUtils.deleteDirectory(new File(Config.getInstance().getScreenshotDir()));
        } catch (IOException e) {
            logger.warn("Can not clean screenshot directory: {} still exist.", Config.getInstance().getScreenshotDir());
        }
        capabilities = new DesiredCapabilities();
        capabilities.setJavascriptEnabled(true);
        capabilities.setCapability("takesScreenshot", true);
        capabilities.setCapability(PhantomJSDriverService.PHANTOMJS_EXECUTABLE_PATH_PROPERTY,
                Config.getInstance().getDriverPath()
        );
    }

    @Before
    public void openBrowser() {
        driver = new PhantomJSDriver(capabilities);
    }

    @After
    public void makeScreenshotCloseBrowser() {
        String screenshotPath = Config.getInstance().getScreenshotDir() +
                "/" + MainTest.class.getSimpleName() + "." + testName.getMethodName() + ".jpg";
        try {
            logger.debug("{}: screenshot path: {}", testName.getMethodName(), screenshotPath);
            File srcFile = ((TakesScreenshot) driver).getScreenshotAs(OutputType.FILE);
            FileUtils.copyFile(srcFile, new File(screenshotPath));
            logger.debug("{}: screenshot saved: {}", testName.getMethodName(), screenshotPath);

        } catch (IOException e) {
            logger.warn("Fail to save screenshot: {}", screenshotPath);
        } finally {
            driver.quit();
        }
    }

    @Test
    public void simpleTest() throws IOException {
        driver.get("http://google.com");
        logger.info("{}: google title: {}", testName.getMethodName(), driver.getTitle());
        assertEquals("Google", driver.getTitle());
    }

}
