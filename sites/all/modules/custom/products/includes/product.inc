<?php
class Product extends Entity {
    public function __construct($values = array()) {
        parent::__construct($values, 'product');
    }

    protected function defaultLabel() {
        return truncate_utf8(strip_tags($this), 30, TRUE, TRUE);
    }

    protected function defaultUri() {
        return array('path' => $this->real_path());
    }

    public function real_path() {
        return PRODUCT_PATH . '/' . $this->id;
    }

}