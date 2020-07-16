package com.sild.autoweb;

import org.apache.commons.io.IOUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Properties;

/**
 * @author Dmitry Korchagin.
 */
public class Config {
    private static final Logger logger = LoggerFactory.getLogger(Config.class);
    private static final String DEFAULT_CONFIG_FILE = "src/test/resources/application.properties";
    private static Config instance = null;
    private String driverPath;
    private String screenshotDir;

    public String getDriverPath() {
        return driverPath;
    }

    public String getScreenshotDir() {
        return screenshotDir;
    }

    public static Config getInstance() {
        if(null == instance) {
            instance = new Config();
        }
        return instance;
    }

    @Override
    public String toString() {
        return "Config{" +
                "driverPath='" + driverPath + '\'' +
                ", screenshotDir='" + screenshotDir + '\'' +
                '}';
    }

    private Config() {
        this(DEFAULT_CONFIG_FILE);
    }

    private Config(String configPath) {
        logger.debug("Load client properties from file: {}", configPath);
        Properties clientProperties = new Properties();
        InputStream stream = null;
        try {
            stream = new FileInputStream(configPath);
            clientProperties.load(stream);
        } catch(Exception e) {
            logger.error("Fail to load client properties");
            e.printStackTrace();
        }finally {
            IOUtils.closeQuietly(stream);
        }

        driverPath = load("driver.path", clientProperties);
        screenshotDir = load("screenshot.dir", clientProperties);
        logger.debug(this.toString());
    }

    private static String load(String name, Properties clientProperties) {
        String value = System.getProperty(name);
        if (value == null) {
            value = clientProperties.getProperty(name);
            if (value == null) {
                    logger.error("Could not find required property: {}", name);
            }
        }
        return value;
    }


}
