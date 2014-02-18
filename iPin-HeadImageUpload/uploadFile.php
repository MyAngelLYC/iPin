<?php
$target_path = "/var/www/iPin/HeadImage/";
$target_path = $target_path. $_FILES['uploadedfile']['name'];
@copy($_FILES['uploadedfile']['tmp_name'],$target_path)
?>
