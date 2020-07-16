<?php

class Config {
    private static $instance = NULL;
    private static $DEFAULT_CREDENTIALS = "credentials.ini";
    private $mysqlHost;
    private $mysqlUser;
    private $mysqlPassword;
    private $mysqlDatabaseName;

    static function getInstance() {
        if (self::$instance == NULL) {
            self::$instance = new Config("local_credentials.ini");
        }
        return self::$instance;
    }

    private function __construct($credentials_path = null) {
        $actual_credentials = "";
        if(null !== $credentials_path && file_exists($credentials_path) ) {
            $actual_credentials = $credentials_path;
        } elseif (file_exists(self::$DEFAULT_CREDENTIALS)) {
            $actual_credentials = self::$DEFAULT_CREDENTIALS;
        } else {
            throw new Exception("Credentials file is absent or is not specified.");
        }
        
        $credentials = parse_ini_file($actual_credentials);
        $this->mysqlHost = $credentials["mysqlHost"];
        $this->mysqlUser = $credentials["mysqlUser"];
        $this->mysqlPassword = $credentials["mysqlPassword"];
        $this->mysqlDatabaseName = $credentials["mysqlDatabaseName"];
    }

    public function getMysqlHost() {
        return $this->mysqlHost;
    }

    public function getMysqlUser() {
        return $this->mysqlUser;
    }

    public function getMysqlPassword() {
        return $this->mysqlPassword;
    }

    public function getMysqlDatabaseName() {
        return $this->mysqlDatabaseName;
    }

}
