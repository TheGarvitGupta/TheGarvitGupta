<?php
	$file = 'lastrun.txt';
	file_put_contents($file, file_get_contents($file) + 0.5);
	header("Location: https://www.garvitgupta.com/run.php");
	die();
?>