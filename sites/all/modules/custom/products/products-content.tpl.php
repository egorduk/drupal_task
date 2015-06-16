<span class="title">Name:
    <?php echo $product['name']; ?>
</span>
<br><br>
Info:
<?php echo $product['info']; ?><br><br>
Type:
<?php echo $product['tp_name']; ?><br><br>
<?php
$image = variable_get('file_public_path', conf_path() . '/files') . '/' . file_uri_target($product['uri']); ?>
<img style="height: 100px" src="../../../../../<?php echo $image; ?>"><br><br>
Created:
<?php echo $product['created_at']; ?><br><br>
Updated:
<?php echo $product['updated_at']; ?><br><br>
<hr>
<br><br>