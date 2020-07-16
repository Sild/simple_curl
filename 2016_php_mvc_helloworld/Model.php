<?php

class Model {

    public function __construct() {
        printf("model inited\n");
        $this->Users = array();
    }

    public function getUsers() {
        $db = DB::getInstance();
        $db->prepare('SELECT * FROM users');
        $response = $db->execute();
        if($response === null) {
            throw new Exception($db->getError());
        }
        return $this->prepareObjects($response, "User");

    }

    public function getUsersByName($name) {
        $db = DB::getInstance();
        $db->prepare('SELECT * FROM users WHERE name = :name');
        $response = $db->execute(array(":name" => $name));
        if($response === null) {
            throw new Exception($db->getError());
        }
        return $this->prepareObjects($response, "User");
    }

    private function prepareObjects($data, $class) {
        $objects = array();
        foreach($data as $row) {
            $objects[] = new $class($row);
        }
        return $objects;
    }

}

class User {
    private $id;
    private $name;
    private $surname;
    private $phone;

    public function __construct($properties) {
        foreach (get_object_vars($this) as $name => $value) {                                                        
            if (isset($properties[$name])) {
                $this->$name = $properties[$name];
            } else {
                $this->$name = null;
            }
        }
    }

    public static function getTableName() {
        return "users";
    }

    public function asTableRow() {
        $row = "<tr>";
        $row .= "<td>$this->id</td>";
        $row .= "<td>$this->name</td>";
        $row .= "<td>$this->surname</td>";
        $row .= "<td>$this->phone</td>";
        $row .= "</tr>";
        return $row;
    }
}
