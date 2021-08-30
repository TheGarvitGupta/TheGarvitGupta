<?php
	$con = mysqli_connect("localhost","garvitgu_C", "Johny!@#PING", "garvitgu_Commute");
	if (mysqli_connect_errno())
	{
		echo "Failed to connect to MySQL: " . mysqli_connect_error();
	}
?>