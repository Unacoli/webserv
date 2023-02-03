<?php
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
    }
}