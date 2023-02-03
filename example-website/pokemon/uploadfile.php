<?php
    $target_dir = "downloads/";
    $target_file = $target_dir . basename($_FILES["fileToUpload"]["tmp_name"]);

    if (isset($_POST["submit"])) 
    {
        echo $target_file;
    }
?>