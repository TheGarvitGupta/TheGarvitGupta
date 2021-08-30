<?php
	$file = fopen("spotify_access_token.txt", "r");
	$spotify_access_token = fgets($file);
	echo $spotify_access_token;
?>