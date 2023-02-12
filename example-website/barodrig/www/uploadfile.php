<?php
if (isset($_FILES['myfile']) && $_FILES['myfile']['error'] == UPLOAD_ERR_OK) {
    $uploadDir = 'Downloads/';
    $uploadFile = $uploadDir . basename($_FILES['myfile']['name']);
    if (move_uploaded_file($_FILES['myfile']['tmp_name'], $uploadFile)) {
        echo "File is valid, and was successfully uploaded.\n";
    }
}
else if (isset($_FILES['myfile']) && $_FILES['myfile']['error'] != UPLOAD_ERR_OK)
{
    if ($_FILES['myfile']['error'] == UPLOAD_ERR_INI_SIZE)
    {
        echo "The uploaded file exceeds the upload_max_filesize directive in php.ini";
    }
    else if ($_FILES['myfile']['error'] == UPLOAD_ERR_FORM_SIZE)
    {
        echo "The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form";
    }
    else if ($_FILES['myfile']['error'] == UPLOAD_ERR_PARTIAL)
    {
        echo "The uploaded file was only partially uploaded";
    }
    else if ($_FILES['myfile']['error'] == UPLOAD_ERR_NO_FILE)
    {
        echo "No file was uploaded";
    }
    else if ($_FILES['myfile']['error'] == UPLOAD_ERR_NO_TMP_DIR)
    {
        echo "Missing a temporary folder";
    }
    else if ($_FILES['myfile']['error'] == UPLOAD_ERR_CANT_WRITE)
    {
        echo "Failed to write file to disk";
    }
    else if ($_FILES['myfile']['error'] == UPLOAD_ERR_EXTENSION)
    {
        echo "File upload stopped by extension";
    }
    else
    {
        echo "Unknown upload error";
    }
}
else
{
    echo "No file uploaded";
}
?>