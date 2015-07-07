<?php
function product_type_page_view($product_type) {
    drupal_set_title(entity_label('product_type', $product_type));
    return entity_view('product_type', array(entity_id('product_type', $product_type) => $product_type), 'full');
}