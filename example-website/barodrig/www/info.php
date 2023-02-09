<?php
  $email = htmlspecialchars($_POST['email']);
  $name  = htmlspecialchars($_POST['name']);

  echo  $email, ' ', $name;
?>