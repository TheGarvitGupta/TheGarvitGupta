<?php
	$file = 'spotify_access_token.txt';
	file_put_contents($file, $_POST["access_token"]);
	echo "Updated the access token to " .  $_POST["access_token"];
	// header("Location: https://www.garvitgupta.com/run.php");
	// die();
?>