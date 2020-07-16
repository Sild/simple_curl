<?php

class View {
    private $tpl;

    public function __construct() {
        printf("view inited\n");
        $this->tpl = file_get_contents("default.tpl");
    }

    public function showUsers(array $Users) {
        $rows = "";
        foreach($Users as $Usr) {
            $rows .= $Usr->asTableRow();
        }
        echo str_replace("[[USER_ROWS]]", $rows, $this->tpl);
    }
}
