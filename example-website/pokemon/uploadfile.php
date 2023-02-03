<?php
    $target_dir = "downloads/";
<<<<<<< HEAD
    $target_file = $target_dir . basename($_FILES["fileToUpload"]["tmp_name"]);

    if (isset($_POST["submit"])) 
    {
        echo $target_file;
=======
    $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);

    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
        echo "The file ". basename( $_FILES["fileToUpload"]["name"]). " has been uploaded.";
    } 
    else {
        echo "Sorry, there was an error uploading your file.";
>>>>>>> 79d8ee8ba255aebd57710f37aaf479cbd68cd683
    }
?>