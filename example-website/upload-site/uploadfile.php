<?php

var_dump($_POST); 
$entityBody = file_get_contents('php://input');
print($entityBody);
var_dump($GLOBALS);
echo $_REQUEST;
echo "<br>";
echo $_POST;
echo "<br>";

// Where the file is going to be placed
$target_path = "Downloads/";

/* Add the original filename to our target path.  
Result is "uploads/filename.extension" */
$target_path = $target_path . basename( $_FILES['file']['name']);

if(move_uploaded_file($_FILES['file']['tmp_name'], $target_path)) {
    echo "The file ".  basename( $_FILES['file']['name']).
    " has been uploaded";
} else{
    echo "There was an error uploading the file, please try again!";
}

?>