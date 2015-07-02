<?php

class ProductController extends EntityAPIController {
    public function create(array $values = array()) {
        $product_type = $values['product_type'];
        $values += array(
            'name' => '',
            'info' => '',
            'created_at' => REQUEST_TIME,
            'updated_at' => REQUEST_TIME,
            'image_id' => 0,
            'type_id' => $product_type->id
        );
        return parent::create($values);
    }

    public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
        dpm($entity);
        $wrapper = entity_metadata_wrapper('product', $entity);
        $content['author'] = array('#markup' => t('Created by: '));
        $content['description'] = array(
            '#theme' => 'field',
            '#weight' => 0,
            '#title' =>t('Description'),
            '#access' => TRUE,
            '#label_display' => 'above',
            '#view_mode' => 'full',
            '#language' => LANGUAGE_NONE,
            '#field_name' => 'field_fake_description',
            '#field_type' => 'text',
            '#entity_type' => 'product',
            '#bundle' => $entity->type,
            '#items' => array(array('value' => $entity->info)),
            '#formatter' => 'text_default',
            0 => array('#markup' => check_plain($entity->info))
        );
        return parent::buildContent($entity, $view_mode, $langcode, $content);
    }
}

class ProductTypeController extends EntityAPIControllerExportable {
    public function create(array $values = array()) {
        $values += array(
            'name' => '',
        );
        return parent::create($values);
    }

    public function save($entity, DatabaseTransaction $transaction = NULL) {
        parent::save($entity, $transaction);
        // Rebuild menu registry. We do not call menu_rebuild directly, but set
        // variable that indicates rebuild in the end.
        // @see http://drupal.org/node/1399618
        variable_set('menu_rebuild_needed', TRUE);
    }
}

class ProductTypeUIController extends EntityDefaultUIController {
    public function hook_menu() {
        $items = parent::hook_menu();
        $items[$this->path]['description'] = 'Manage Task types.';
        return $items;
    }
}

class Product extends Entity {
    protected function defaultLabel() {
        return $this->name;
    }

    protected function defaultUri() {
        return array('path' => 'product/' . $this->identifier());
    }
}

class ProductType extends Entity {
    public $name;
    public $label;
    public $weight = 0;

    public function __construct($values = array()) {
        parent::__construct($values, 'product_type');
    }

    function isLocked() {
        return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
    }
}