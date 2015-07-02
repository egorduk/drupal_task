<?php
function product_admin_page_add() {
    $items = array();
    foreach (product_types() as $task_type_key => $task_type) {
        $items[] = l(entity_label('product_type', $task_type), 'product/add/' . $task_type_key);
    }
    return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of product to create.')));
}

function product_add($type) {
    $product_type = product_types($type);
    $product = entity_create('product', array('product_type' => $product_type));
    //dpm($product);
   // drupal_set_title(t('Create @name', array('@name' => entity_label('product_type', $product_type))));
    $output = drupal_get_form('product_form', $product);
    return $output;
}

function product_form($form, &$form_state, $product) {
    //dpm($product);
    $form_state['product'] = $product;
    $form['name'] = array(
        '#type' => 'textfield',
        '#required' => TRUE,
        '#title' => t('Name'),
        '#default_value' => $product->name,
    );
    $form['info'] = array(
        '#type' => 'textarea',
        '#required' => TRUE,
        '#title' => t('Info'),
        '#default_value' => $product->info,
    );
    $form['type'] = array(
        '#type' => 'value',
        '#value' => $product->type_id,
    );
    $form['img'] = array(
        '#title'         => t('Product image'),
        '#description'   => t(''),
        '#type'          => 'file',
        //'#default_value' => $product ? $product['img'] : '',
        '#required'      => false
    );
    //field_attach_form('product', $task, $form, $form_state);
    $submit = array();
    if (!empty($form['#submit'])) {
        $submit += $form['#submit'];
    }
    $form['actions'] = array(
        '#weight' => 100,
    );
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Create new product'),
        '#submit' => $submit + array('product_form_submit'),
    );
    // Show Delete button if we edit task.
    /*$task_id = entity_id('example_task' ,$task);
    if (!empty($task_id) && example_task_access('edit', $task)) {
        $form['actions']['delete'] = array(
            '#type' => 'submit',
            '#value' => t('Delete'),
            '#submit' => array('example_task_form_submit_delete'),
        );
    }*/
    $form['#validate'][] = 'product_form_validate';
    return $form;
}

function product_form_validate($form, &$form_state) {
}

function product_form_submit($form, &$form_state) {
    /*$product = $form_state['values']['product'];
    $product->name = $form_state['values']['name'];
    $product->changed = REQUEST_TIME;
    //field_attach_submit('product', $product, $form, $form_state);
    $is_new = !empty($product->is_new) ? TRUE : FALSE;
    $product->save();
    if ($is_new) {
        drupal_set_message(t('%title has been created.', array('%title' => $product->name)));
    } else {
        drupal_set_message(t('%title has been updated.', array('%title' => $product->name)));
    }
    $form_state['redirect'] = array('product/' . $product->id);*/
    $product = $form_state['product'];
    entity_form_submit_build_entity('product', $product, $form, $form_state);
    product_save($product);
    $product_uri = entity_uri('product', $product);
    //dpm($product_uri);
    $form_state['redirect'] = $product_uri['path'];
    drupal_set_message(t('Product %title created', array('%title' => entity_label('product', $product))));
}

function product_form_delete_submit($form, &$form_state) {
    $destination = array();
    if (isset($_GET['destination'])) {
        $destination = drupal_get_destination();
        unset($_GET['destination']);
    }
    $product = $form_state['values']['product'];
    $form_state['redirect'] = array('product/' . $product->id . '/delete', array('query' => $destination));
}

function product_delete_confirm($form, &$form_state, $product) {
    $form['product_id'] = array('#type' => 'value', '#value' => $product->id);
    return confirm_form($form,
        t('Are you sure you want to delete %title?', array('%title' => $product->title)),
        'products',
        t('This action cannot be undone.'),
        t('Delete'),
        t('Cancel')
    );
}

function product_delete_confirm_submit($form, &$form_state) {
    if ($form_state['values']['confirm']) {
        $product = product_load($form_state['values']['product_id']);
        product_delete($form_state['values']['product_id']);
        drupal_set_message(t('%title has been deleted.', array('%title' => $product->title)));
    }
    $form_state['redirect'] = 'admin/config/content/products';
}

function product_type_form($form, &$form_state, $task_type, $op = 'edit') {

    if ($op == 'clone') {
        $task_type->label .= ' (cloned)';
        $task_type->type = '';
    }

    $form['label'] = array(
        '#title' => t('Label'),
        '#type' => 'textfield',
        '#default_value' => $task_type->label,
        '#description' => t('The human-readable name of this task type.'),
        '#required' => TRUE,
        '#size' => 30,
    );

    // Machine-readable type name.
    $form['type'] = array(
        '#type' => 'machine_name',
        '#default_value' => isset($task_type->type) ? $task_type->type : '',
        '#maxlength' => 32,
        '#disabled' => $task_type->isLocked() && $op != 'clone',
        '#machine_name' => array(
            'exists' => 'example_task_types',
            'source' => array('label'),
        ),
        '#description' => t('A unique machine-readable name for this task type. It must only contain lowercase letters, numbers, and underscores.'),
    );

    $form['description'] = array(
        '#type' => 'textarea',
        '#default_value' => isset($task_type->description) ? $task_type->description : '',
        '#description' => t('Description about the task type.'),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save task type'),
        '#weight' => 40,
    );

    if (!$task_type->isLocked() && $op != 'add' && $op != 'clone') {
        $form['actions']['delete'] = array(
            '#type' => 'submit',
            '#value' => t('Delete task type'),
            '#weight' => 45,
            '#limit_validation_errors' => array(),
            '#submit' => array('example_task_type_form_submit_delete')
        );
    }
    return $form;
}

/**
 * Submit handler for creating/editing task_type.
 */
function product_type_form_submit(&$form, &$form_state) {
    $task_type = entity_ui_form_submit_build_entity($form, $form_state);
    // Save and go back.
    example_task_type_save($task_type);

    // Redirect user back to list of task types.
    $form_state['redirect'] = 'admin/structure/task-types';
}

function product_type_form_submit_delete(&$form, &$form_state) {
    $form_state['redirect'] = 'admin/structure/task-types/' . $form_state['example_task_type']->type . '/delete';
}

/**
 * Task type delete form.
 */
function product_type_form_delete_confirm($form, &$form_state, $task_type) {
    $form_state['task_type'] = $task_type;
    // Always provide entity id in the same form key as in the entity edit form.
    $form['task_type_id'] = array('#type' => 'value', '#value' => entity_id('example_task_type' ,$task_type));
    return confirm_form($form,
        t('Are you sure you want to delete task type %title?', array('%title' => entity_label('task_type', $task_type))),
        'task/' . entity_id('example_task_type' ,$task_type),
        t('This action cannot be undone.'),
        t('Delete'),
        t('Cancel')
    );
}

/**
 * Task type delete form submit handler.
 */
function product_type_form_delete_confirm_submit($form, &$form_state) {
    $task_type = $form_state['task_type'];
    example_task_type_delete($task_type);

    watchdog('example_task_type', '@type: deleted %title.', array('@type' => $task_type->type, '%title' => $task_type->label));
    drupal_set_message(t('@type %title has been deleted.', array('@type' => $task_type->type, '%title' => $task_type->label)));

    $form_state['redirect'] = 'admin/structure/task-types';
}