<?php
class Controller {
    private $view;
    private $model;

    public function run_example() {
        $this->view = new View();
        $this->model = new Model();
        $this->showUsers();
    }

    private function showUsers() {
        $Users = $this->model->getUsersByName("name3");
        $this->view->showUsers($Users);
    }
}
