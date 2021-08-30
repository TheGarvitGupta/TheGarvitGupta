<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php
	/* Evaluate Parameters */
	$timestamp = time();
	$offerid = generateRandomString();
	$offererid = $_SESSION["UserID"];

	/* POST Parameters */
	$from = htmlspecialchars(utf8_encode($_POST["from"]));
	$to = htmlspecialchars(utf8_encode($_POST["to"]));
	$date = htmlspecialchars(utf8_encode($_POST["date"]));
	$time = htmlspecialchars(utf8_encode($_POST["time"]));
	$seats = htmlspecialchars(utf8_encode($_POST["seats"]));
	$status = "active";

	if ($seats == 0)
	{
		$status	= 'hidden';
	}

	/* Random string generator */
	function generateRandomString($length = 10) 
	{
		$characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
	    $charactersLength = strlen($characters);
	    $randomString = '';
	    for ($i = 0; $i < $length; $i++)
	    {
	        $randomString .= $characters[rand(0, $charactersLength - 1)];
	    }
	    return $randomString;
	}

	/* Check if date is valid */
    $x = date_parse_from_format('Y F j', $date);

	if (!($x["error_count"] == 0) || ($from == $to))
	{
		echo 'error2';
		exit();
	}
	else /* Details are correct */ /* Echo SUCCESS */
	{
		/* Build timestamp from date */
		$dateStamp = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);

		/* insert to AberLift_OFFERS */
		include 'DB_Connect.php';

		$sql = "INSERT INTO AberLift_OFFERS VALUES('$timestamp', '$offerid', '$offererid', '$from', '$to', '$dateStamp', '$time', '$seats', '$status')";
		if (!mysqli_query($con,$sql))
		{
			echo("Error description: " . mysqli_error($con));
		}

		/* return */
		echo 'success';
	}
?>