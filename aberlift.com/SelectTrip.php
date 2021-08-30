<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php
# SMS
    $user = "lifterapp";
    $password = "dGRBeZTAXCFSWP";
    $api_id = "3524842";
    $baseurl ="http://api.clickatell.com";
#/SMS

	include 'DB_Connect.php';
	$offerID = htmlspecialchars(utf8_encode($_POST["offerID"]));
	$SQL = "SELECT * FROM AberLift_OFFERS WHERE offer_id = '$offerID'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	$Result = mysqli_query($con,$SQL);

	$row = mysqli_fetch_array($Result);

	$DriverID = $row['offerer_id'] . '%';

	$SQL = "SELECT * FROM Commute_USERS WHERE Random LIKE '$DriverID'";	
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	$Result = mysqli_query($con,$SQL);

	$row = mysqli_fetch_array($Result);

	$DriverName = $row['Name'];
	$DriverPhone = $row['Phone'];
	$DriverEmail = $row['Email'];

	$RiderID = $_SESSION["UserID"];

	$SQL = "SELECT * FROM Commute_USERS WHERE Random = '$RiderID'";	
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	$Result = mysqli_query($con,$SQL);

	$row = mysqli_fetch_array($Result);

	$RiderName = $row['Name'];
	$RiderPhone = $row['Phone'];
	$RiderEmail = $row['Email'];


	$Text = "Hi <b>$RiderName</b>,<br><br>Your ride has been confirmed. Please find below, the information of the person who will be driving you:<br><br><b>Name: </b>$DriverName<br><b>Phone: </b>$DriverPhone<br><b>Email Address: </b>$DriverEmail<br><br>We have also sent them your details. Have a happy and safe travel!<br><br>The Aber Team";

	$From = "AberLift";
	$Header .= "From: AberLift <noreply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($RiderEmail,"Aber - Ride Confirmation",$Text,$Header);

	#Rider text start
 
    $text = urlencode("Hi $RiderName, Your ride has been confirmed. Driver details: $DriverName - $DriverPhone - $DriverEmail - We have also sent them your details. Have a happy and safe travel! - The LifterApp Team");
    $torider = $RiderPhone;

    $url = "$baseurl/http/sendmsg?user=$user&password=$password&api_id=$api_id&to=$torider&text=$text";

	$returned = @file_get_contents($url, false);

    #Rider over 

	$Text = "Hi <b>$DriverName</b>,<br><br>A person has accepted your offer for the ride. Please find their information below:<br><br><b>Name: </b>$RiderName<br><b>Phone: </b>$RiderPhone<br><b>Email Address: </b>$RiderEmail<br><br>We have also sent them your details. Have a happy and safe travel!<br><br>The Aber Team";

	$From = "AberLift";
	$Header .= "From: AberLift <noreply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($DriverEmail,"Aber - Ride Confirmation",$Text,$Header);

	#Driver text start
 
    $text = urlencode("Hi $DriverName, A person has accepted your offer for the ride. Rider details: $RiderName - $RiderPhone - $RiderEmail - We have also sent them your details. Have a happy and safe travel! - The LifterApp Team");
    $todriver = $DriverPhone;

    $url = "$baseurl/http/sendmsg?user=$user&password=$password&api_id=$api_id&to=$todriver&text=$text";

	$returned = @file_get_contents($url, false);

    #Driver over 

	echo '<img src="Resources/svg/check.svg" style="min-width:200px;min-height:200px;margin-top:50px;margin-left:300px;margin-right:300px;"></div><div style="margin-left: 100px;margin-right: 100px;margin-top: 25px;color: rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;line-height: 18px;font-size: 14px;"><div style="text-align:center;font-size:20px;" class="accent">Yay! Your ride from A to B has been confirmed.</div><br>We\'ll shortly mail you the details of your ride and the offerer. We would also notify GARVIT that you have accepted the offer.</div><div onclick="cancelOffer()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
?>