<?php
	$con = mysqli_connect("localhost","garvitgu_C", "Johny!@#PING", "garvitgu_Commute");

	if (mysqli_connect_errno())
  	{
  		echo "Failed to connect to MySQL: " . mysqli_connect_error();
  	}

	$sql = "CREATE TABLE aberlift_requestedRides (request_id CHAR(10), passenger_id CHAR(10), fromLocation_code CHAR(2), toLocation_code CHAR(2), ride_date CHAR(20), ride_time CHAR(5))";
	
	if (!mysqli_query($con,$sql))
	{
		echo("Error description: " . mysqli_error($con));
	}
	/*
	$sql = "INSERT INTO Commute_USERS VALUES('$UserName', '$Password', '$Phone', '$Email', '$Random', 'N')";
	if (!mysqli_query($con,$sql))
	{
		echo("Error description: " . mysqli_error($con));
	}
	*/
?>