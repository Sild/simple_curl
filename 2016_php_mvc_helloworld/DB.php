<?php

class DB {
    static private $instance = NULL;
    private $dbh;
    private $error;
    private $st;
    static function getInstance() {
        if (self::$instance == NULL) {
            self::$instance = new DB();
        }
        return self::$instance;
    }

    private function __construct() {
        $this->connect(
            Config::getInstance()->getMysqlHost(), 
            Config::getInstance()->getMysqlUser(), 
            Config::getInstance()->getMysqlPassword(), 
            Config::getInstance()->getMysqlDatabaseName()
        );
        $this->error = null;
    }

    private function connect($server = 'localhost', $login = 'root', $password = '', $dbname = '') {
        try {
            $this->dbh = new PDO("mysql:host=$server;dbname=$dbname", $login, $password, array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8"));
        } catch (PDOException $e) {
            print "Error!: " . $e->getMessage() . "<br/>";
            die();
        }
    }

    public function getLastInsertId() {
        return $this->dbh->lastInsertId();
    }

    public function getError() {
        return $this->error;
    }

    public function __destruct() {
        $this->disconnect();
    }

    public function disconnect() {
        $this->dbh = null;
    }

    public function prepare($sql) {
        $this->st = $this->dbh->prepare($sql);
    }

    public function execute($binds = array()) {
        $this->st->execute($binds);

        if ($this->st->errorCode() != 0) {
            $this->error = $this->st->errorInfo()[2];
            return null;
        }
        $this->error = null;
        $response = $this->st->fetchAll(PDO::FETCH_ASSOC);
        $this->st = null;
        return $response;
    }

}
