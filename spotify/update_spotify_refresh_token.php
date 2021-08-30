<?php
	$file = 'spotify_refresh_token.txt';
	file_put_contents($file, $_POST["refresh_token"]);
	// header("Location: https://www.garvitgupta.com/run.php");
	// die();
?>