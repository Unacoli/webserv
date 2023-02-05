<?php
<<<<<<< HEAD
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
=======
if (isset($_FILES['file']))
{
    $uploaddir = 'Downloads/';
    $uploadfile = $uploaddir . basename($_FILES['userfile']['name']);
    echo "Filename: " . $_FILES['file']['name']."<br>";
    echo "Type : " . $_FILES['file']['type'] ."<br>";
    echo "Size : " . $_FILES['file']['size'] ."<br>";
    echo "Temp name: " . $_FILES['file']['tmp_name'] ."<br>";
    echo "Error : " . $_FILES['file']['error'] . "<br>";
    if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
        echo "File is valid, and was successfully uploaded.\n";
>>>>>>> e6eddbcfab012d2a039e900ee6d9f8e1d5e889f7
    }
}