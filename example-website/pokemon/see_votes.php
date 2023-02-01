<?php
$json = file_get_contents('/data/data.json');
header('Content-Type: application/json');
echo $json;