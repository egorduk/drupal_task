<?php

/**
 * Task view callback.
 */
function example_task_view($product) {
    drupal_set_title(entity_label('product', $product));
    return entity_view('product', array(entity_id('product', $product) => $product), 'full');
}
