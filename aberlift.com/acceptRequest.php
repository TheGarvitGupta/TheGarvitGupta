<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php

	/* This script will be executed by the offerer who accepts an already available request from the list. He will send the request ID of the request and his own ID */

	include 'DB_Connect.php';

	$requestID = htmlspecialchars(utf8_encode($_POST["requestID"]));
	$offererID = $_SESSION["UserID"];

	// get request details

	$SQL = "SELECT * FROM AberLift_REQUESTS WHERE request_id = '$requestID'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);

	$fromLocation = $row['from_code'];
	$toLocation = $row['to_code'];
	$rideDate = $row['ride_date'];
	$rideTime = $row['ride_time'];
	$timestamp = time();

	$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$fromLocation'";
	$resultint = mysqli_query($con, $Query);
	if (!empty($resultint))
	{
		$rowint = mysqli_fetch_array($resultint);
		$fromName = $rowint['Station_Name'];
	}

	$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$toLocation'";
	$resultint = mysqli_query($con, $Query);
	if (!empty($resultint))
	{
		$rowint = mysqli_fetch_array($resultint);
		$toName = $rowint['Station_Name'];
	}

    $x = date_parse_from_format('Hi', $rideTime);
	$timeInEpoch = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);
	$formattedTime = date('H:i', $timeInEpoch);

	$formattedDate = date('j F Y', $rideDate);


	// check if request is still active or taken
	$SQL = "SELECT * FROM AberLift_REQUESTS WHERE request_id='$requestID'";
	$resultint = mysqli_query($con, $SQL);

	$rowint = mysqli_fetch_array($resultint);
	$status = $rowint['status'];

	if ($status != "active")
	{
		echo "<img src='Resources/svg/warning.svg' style='min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;'></div><div style='margin-left: 100px;margin-right: 100px;margin-top: 10px;color: rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;line-height: 18px;font-size: 14px;'><div style='text-align:center;font-size:18px;line-height:32px' class='accent'>Already taken</div><br>It seems that this request is no more available. This might be because someone has already offered them a lift or the request has been deleted.</div><div onclick='cancelRequest()' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;'>close</div>";
		exit();
	}

	// update requests table with status as taken

	$SQL = "UPDATE AberLift_REQUESTS SET status='taken' WHERE request_id='$requestID'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	// generate offer id
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

	$newOfferID = generateRandomString();

	// insert into offer table against offerer id

	$SQL = "INSERT INTO AberLift_OFFERS VALUES('$timestamp', '$newOfferID', '$offererID', '$fromLocation', '$toLocation', '$rideDate', '$rideTime', '0' , 'taken')";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	
	// generate trip id

	$tripID = generateRandomString();

	// insert trip id, request id and offer id
	$SQL = "INSERT INTO AberLift_TRIPS VALUES('$timestamp', '$tripID', '$requestID', '$newOfferID', 'positive')";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}


	// communication data
	// offerer
	$SQL = "SELECT * FROM AberLift_USERS WHERE userID = '$offererID'";	
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);

	$offererName = $row['firstName'] . ' ' .  $row['lastName'];
	$offererPhone = $row['phoneNumber'];
	$offererEmail = $row['emailAddress'];

	// requester
	// we have the requestID
	$SQL = "SELECT * FROM AberLift_USERS WHERE userID = (SELECT requester_id FROM AberLift_REQUESTS WHERE request_id='$requestID')";	
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);

	$requesterName = $row['firstName'] . ' ' .  $row['lastName'];
	$requesterPhone = $row['phoneNumber'];
	$requesterEmail = $row['emailAddress'];

	// mail requester

	$Text = "<div style='color:rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;font-size:14px;'><div style='font-size:24px;'>Hi $requesterName,</div><div style='margin-top:10px;'>Your request for a lift from $fromName to $toName has been confirmed! $offererName has agreed to share their lift with you. Please find the relevent details below.</div><div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3;color:white;font-weight:500;font-size:16px;text-align:center;padding:5px;height:16px;line-height:16px;'>OFFERER DETAILS</div><b>Name: </b>$offererName<br><b>Phone: </b>$offererPhone<br><b>Email Address: </b>$offererEmail<div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3 ;color:white;font-weight:500;font-size:15px;text-align:center;padding:4px;height:16px;line-height:16px;'>TRIP DETAILS</div><b>Boarding Point:</b> $fromName<br><b>Destination:</b> $toName<br><b>Date:</b> $formattedDate<br><b>Time:</b> $formattedTime<br><br><div style='color:#2196F3;'>The AberLift Team</div></div><div style='font-size:12px;color:rgba(0,0,0,0.30);margin-top:25px;line-height:18px;-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;'>This is an automatically generated mail. Please do not reply. For support, please write to us at <a href='mailto:support@aberlift.com'>support@aberlift.com</a>. This email was sent to $requesterEmail. Please add no-reply@aberlift.com to your contacts so that our emails can reach to you without any troubles in the future.</div>";

	$Header = "From: AberLift <no-reply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($requesterEmail,"AberLift - Request Approved",$Text,$Header);
	
	// message requester

    $user = "lifterapp";
    $password = "dGRBeZTAXCFSWP";
    $api_id = "3524842";
    $baseurl ="http://api.clickatell.com";

    $text = urlencode("Hi $requesterName,\nYour request for a lift from $fromName to $toName on $formattedDate has been accepted by $offererName" . " (" . $requesterPhone . ")" . "\nPlease check your mail for details \n- The AberLift Team");
    $torider = $requesterPhone;
    $url = "$baseurl/http/sendmsg?user=$user&password=$password&api_id=$api_id&to=$torider&text=$text";
	@file_get_contents($url, false);

	// mail offerer

	$Text = "<div style='color:rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;font-size:14px;'><div style='font-size:24px;'>Hi $offererName,</div><div style='margin-top:10px;'>We've just sent a text to $requesterName saying that you have agreed share your ride with them from $fromName to $toName" . ". Please find the relevent details below.</div><div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3;color:white;font-weight:500;font-size:16px;text-align:center;padding:5px;height:16px;line-height:16px;'>REQUESTER DETAILS</div><b>Name: </b>$requesterName<br><b>Phone: </b>$requesterPhone<br><b>Email Address: </b>$requesterEmail<div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3 ;color:white;font-weight:500;font-size:15px;text-align:center;padding:4px;height:16px;line-height:16px;'>TRIP DETAILS</div><b>Boarding Point:</b> $fromName<br><b>Destination:</b> $toName<br><b>Date:</b> $formattedDate<br><b>Time:</b> $formattedTime<br><br><div style='color:#2196F3;'>The AberLift Team</div></div><div style='font-size:12px;color:rgba(0,0,0,0.30);margin-top:25px;line-height:18px;-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;'>This is an automatically generated mail. Please do not reply. For support, please write to us at <a href='mailto:support@aberlift.com'>support@aberlift.com</a>. This email was sent to $offererEmail. Please add no-reply@aberlift.com to your contacts so that our emails can reach to you without any troubles in the future.</div>";

	$Header = "From: AberLift <no-reply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($offererEmail,"AberLift - Offer Confirmation",$Text,$Header);

	// echo reply
	echo "<img src='Resources/svg/check.svg' style='min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;'></div><div style='margin-left: 100px;margin-right: 100px;margin-top: 10px;color: rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;line-height: 18px;font-size: 14px;'><div style='text-align:center;font-size:18px;line-height:32px' class='accent'>Awesome! Your offer has been shared</div><br>We'll shortly mail you $requesterName" . "'s details. We would also notify them that you have accepted the request.</div><div onclick='cancelRequest()' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;'>close</div>";
?>