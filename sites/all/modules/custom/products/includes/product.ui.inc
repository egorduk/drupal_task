<?php
class ProductContentUIController extends EntityDefaultUIController {
    function hook_menu() {
        $items = array();
        $this->id_count = count(explode('/', $this->path));
        $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_object';
        dpm($wildcard);
        $items[$this->path] = array(
            'title callback' => '',
            'page callback' => 'product_view_messages',
            'access callback' => TRUE,
            //'access callback' => 'public_access',
            'access arguments' => array('view'),
            'file' => 'includes/product.pages.inc',
            'file path' => drupal_get_path('module', 'products'),
            'menu_name' => 'user-menu',
        );
        $items[$this->path . '/' . $wildcard . '/all'] = array(
            'title callback' => 'entity_label',
            'title arguments' => array($this->entityType, $this->id_count),
            'page callback' => 'entity_ui_entity_page_view',
            'file' => 'includes/entity.ui.inc',
            'file path' => drupal_get_path('module', 'entity'),
            'page arguments' => array($this->id_count),
            'load arguments' => array($this->entityType),
            'access callback' => 'product',
            'access arguments' => array('view', $this->id_count),
        );
        $items[$this->path . '/' . $wildcard . '/view'] = array(
            'title'            => 'Products',
            'description'      => 'Configure',
            'page callback'    => 'products_list',
            'access arguments' => array('administer site configuration'),
        );
        // add form
        $items[$this->path . '/add'] = array(
            'title'            => 'Add product',
            'page callback'    => 'product_get_form',
            'page arguments'   => array('create'),
            'load arguments' => array($this->entityType),
            //'access callback' => 'product_access',
            'access callback' => TRUE,
            'access arguments' => array('create'),
            'type'             => MENU_LOCAL_TASK,
            'weight'           => 2,
        );
        // edit form
        $items[$this->path . '/' . $wildcard . '/edit'] = array(
            'title'            => 'Edit products',
            'page callback'    => 'product_get_form',
            'page arguments'   => array('edit', $this->id_count),
            'load arguments' => array($this->entityType),
            'access callback' => 'product_access',
            'access arguments' => array('edit', $this->id_count),
            'type'             => MENU_CALLBACK,
        );
        // delete
        $items[$this->path . '/' . $wildcard . '/delete'] = array(
            'title'            => 'Delete product',
            'page arguments'   => array('product_get_form', $this->id_count),
            'load arguments' => array($this->entityType),
            'page callback' => 'drupal_get_form',
            'access arguments' => array('delete', $this->id_count),
            'type'             => MENU_CALLBACK,
        );
        return $items;
    }

    function product_get_form($op = 'create', $product = NULL, $recipient = NULL) {
        $form_state = array(
            'entity_type' => 'product',
            'op' => $op,
        );
        $form_state['product'] = $product;
        switch ($op) {
            case 'create':
                if (isset($recipient)) {
                    drupal_set_title(t('New message to user %name', array('%name' => $recipient->name)), PASS_THROUGH);
                }
                $form_state['product'] = product_create();
                break;
            case 'edit':
                $recipient = user_load($product->id);
                break;
            default:
                break;
        }
        form_load_include($form_state, 'inc', 'product', 'includes/product.ui');
        // Поскольку drupal_retrieve_form() жить без аргментов для функции формы
        // не может, приходится формировать их даже в том случае,
        // когда нам они не нужны.
        $form_state['build_info']['args'] = array($op, $product, $recipient);
        return drupal_build_form('product_form', $form_state);
    }

    function product_form($form, &$form_state) {
        return entity_ui_controller($form_state['entity_type'])->form($form, $form_state);
    }

    function product_form_validate($form, &$form_state) {
        entity_ui_controller($form_state['entity_type'])->formValidate($form, $form_state);
    }

    function product_form_submit($form, &$form_state) {
        entity_ui_controller($form_state['entity_type'])->formSubmit($form, $form_state);
    }

    function form($form, &$form_state) {
        //$options = _get_type_product_items('selector');
        $form['name'] = array(
            '#title'         => t('Product name'),
            '#description'   => t(''),
            '#type'          => 'textfield',
            //'#default_value' => $product ? $product['name'] : '',
            '#required'      => true,
        );
        $form['info'] = array(
            '#title'         => t('Product info'),
            '#description'   => t(''),
            '#type'          => 'textarea',
            //'#default_value' => $product ? $product['info'] : '',
            '#required'      => true
        );
        $form['img'] = array(
            '#title'         => t('Product image'),
            '#description'   => t(''),
            '#type'          => 'file',
            //'#default_value' => $product ? $product['img'] : '',
            '#required'      => false
        );
        /*$form['type_product_id'] = array(
            '#title'         => t('Type product'),
            '#type'          => 'select',
            '#description'   => t(''),
            //'#default_value' => $typeProduct ? $product['type_id'] : t('Select type product'),
            '#options'       => $options,
            '#required'      => true
        );*/
        $form['submit'] = array(
            '#type'  => 'submit',
            //'#value' => $product ? t('Save') : t('Add')
        );
        /*if ($product) {
            $form['id'] = array(
                '#type'  => 'value',
                '#value' => $product['id']
            );
        }*/
        return $form;
    }

    function formValidate($form, &$form_state) {
        $file = file_save_upload('img', array(
            'file_validate_is_image' => array(),
            'file_validate_extensions' => array('png gif jpg jpeg')
        ), "public://");
        if ($file) {
            if ($file = file_move($file, 'public://')) {
                $form_state['storage']['file'] = $file;
            } else {
                form_set_error('img', t("Failed to write the uploaded file to the file folder!"));
            }
        } else {
            form_set_error('img', t('No file was uploaded!'));
        }
        if (!$form_state['values']['info']) {
            form_set_error('info', t('Product info field is required.!'));
        }
    }

    function formSubmit($form, &$form_state) {
        $file = $form_state['storage']['file'];
        unset($form_state['storage']['file']);
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);
        //dpm($file);
        $product = array(
            'name'       => $form_state['values']['name'],
            'info'       => $form_state['values']['info'],
            'image_id'   => $file->fid,
            'type_id'    => $form_state['values']['type_product_id'],
            'created_at' => time(),
            'updated_at' => time(),
        );
        // save edit data
        if (isset($form_state['values']['id'])) {
            $product['id'] = $form_state['values']['id'];
            drupal_write_record('product', $product, 'id');
            drupal_set_message(t('Product edited!'));
        } // add new data
        else {
            drupal_write_record('product', $product);
            drupal_set_message(t('Product added!'));
        }
        //drupal_goto('admin/config/content/products');
    }
}