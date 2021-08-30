<?php

	/* If JavaScript is disabled */
	echo '<noscript><div style="position:fixed;top:0px;left:0px;right:0px;bottom:0px;background-color:white;height:75%;width:50%;padding-left:25%;padding-right:25%;z-index:101;background-image:url(\'Resources/images/AberLift_Logo.png\');background-repeat:no-repeat;background-size:contain;background-position:bottom center;"></div><div style="position:fixed;top:75%;z-index: 100;height:25%;width:50%;background-color:white;padding-left:25%;padding-right:25%;text-align:center;font-family: Roboto, Helvetica,Arial;font-size:1.5em;"><a target="_blank" style="text-decoration:none;" href="http://www.wikihow.com/Turn-on-Javascript-in-Internet-Browsers"><div class="header-button" style="display:inline-block;padding:10px;">Please enable JavaScript to view this site.</div></a></div></noscript>';

	/* Start */
	if (isset($_SESSION["LoggedIn"]))
	{
		$firstName = $_SESSION["FirstName"];
		$lastName = $_SESSION["LastName"];
		$picture = 'uploads/' . $_SESSION["Picture"];

		if (strpos($picture, "?refresh=") >= 10)
		{
			$pictureName = substr($picture, 0, strpos($picture, "?refresh="));
		}
		else
		{
			$pictureName = $picture;
		}

		if (!(file_exists($pictureName)))
		{
			$picture = 'Resources/images/person_generic.png';
		}	

		echo '<header>';
		echo '<div class="header-container">';
		echo '<a href="index.php">';
		echo '<div style="height:65px;float:left;">';
		echo '<div class="logo-icon"></div>';
		echo '<div class="logo-text">AberLift</div>';
		echo '</div>';
		echo '</a>';
		echo '<div onclick="toggleMenu();" class="headerUserBG">';
		echo '<div class="headerUserName">' . $firstName . ' ' . $lastName . '</div>';
		echo '<div class="headerUserPicture" style="background:url(\'' . $picture . '\');background-size: cover;background-position: center;background-repeat:no-repeat;"></div>';
		echo '</div>';

		/* Language Option */
		echo '<div class="language">';
		echo '<select id="language" class="language-selector">';
		echo '<option value="EN">English</option>';
		echo '<option value="WE">Welsh</option>';
		echo '</select>';
		echo '<div class="language-image"></div>';
		echo '</div>';

		echo '</div>';
		echo '</header>';
		echo '<div id="headerScrollBox" class="headerScroll">';
		echo '<div class="headerScrollItemsHidden">' . $firstName . ' ' . $lastName . '</div>';
		echo '<a style="text-decoration:none" href="profile.php"><div class="headerScrollItems">Profile</div></a>';
		echo '<a style="text-decoration:none" href="trips.php"><div class="headerScrollItems">All Lifts</div></a>';
		echo '<a style="text-decoration:none" href="signout.php"><div class="headerScrollItems">Sign out</div></a>';
		echo '</div>';
		echo '<div onclick="hideMenu()" id="boxContainer"></div>';
	}
	else
	{
		echo '<header>';
		echo '<div class="header-container">';
		echo '<a href="index.php">';
		echo '<div style="height:65px;float:left;">';
		echo '<div class="logo-icon"></div>';
		echo '<div class="logo-text">AberLift</div>';
		echo '</div>';
		echo '</a>';
		echo '<a href="signup.php"><div class="sign-up header-button">Sign Up</div></a>';
		echo '<a href="signin.php"><div class="sign-in header-button">Sign In</div></a>';
		
		/* Language Option */
		echo '<div class="language">';
		echo '<select id="language" class="language-selector">';
		echo '<option value="EN">English</option>';
		echo '<option value="WE">Welsh</option>';
		echo '</select>';
		echo '<div class="language-image"></div>';
		echo '</div>';

		echo '</div>';
		echo '</header>';
	}
?>

<script type="text/javascript">
	$(document).on('change','#language',function(){ changeLanguage(); });

	if (getCookie("language") == "WE")
	{
		$("#language").val('WE');
	}
	else
	{
		$("#language").val('EN');
	}

	function changeLanguage()
	{
		cvalue = $("#language").val();
	    document.cookie = "language="+cvalue+";  expires=Wed, 1 Jan 2020 12:00:00 UTC";
	    location.reload();
	}

	function getCookie(cname) {
	    var name = cname + "=";
	    var ca = document.cookie.split(';');
	    for(var i=0; i<ca.length; i++) {
	        var c = ca[i];
	        while (c.charAt(0)==' ') c = c.substring(1);
	        if (c.indexOf(name) == 0) return c.substring(name.length,c.length);
	    }
	    return "";
	}
</script>