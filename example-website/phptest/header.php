<?php

// start session
session_start();

// set page title
if(!isset($page_title)) {
    $page_title = "Default";
}

// include top part of the HTML document
echo "<!DOCTYPE html>
<html>
<head>
    <title>{$page_title}</title>
</head>
<body>";

?>