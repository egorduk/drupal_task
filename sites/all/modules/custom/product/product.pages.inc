<?php
function product_page_view($product) {
    drupal_set_title(entity_label('product', $product));
    return entity_view('product', array(entity_id('product', $product) => $product), 'full');
}

function products_page_view() {
    //drupal_set_title(entity_label('product', $product));
    return entity_view('product', array(entity_id('product', $product) => $product), 'full');
}