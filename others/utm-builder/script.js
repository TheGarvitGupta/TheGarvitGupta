/* Social Organic */

$("#field-4").change(function() { setSocialUTM(); });
$("#field-4").keyup(function() { setSocialUTM(); });
$("#field-5").change(function() { setSocialUTM(); });
$("#field-5").keyup(function() { setSocialUTM(); });

function setSocialUTM() {
	urlutmfacebook = $("#field-4").val() + "?utm_source=facebook&utm_medium=social&utm_content=" + $("#field-5").val().replace(/ /g,'-') + "&utm_campaign=CC&lang=en";
	urlutmtwitter = $("#field-4").val() + "?utm_source=twitter&utm_medium=social&utm_content=" + $("#field-5").val().replace(/ /g,'-') + "&utm_campaign=CC&lang=en";
	urlutmlinkedin = $("#field-4").val() + "?utm_source=linkedin&utm_medium=social&utm_content=" + $("#field-5").val().replace(/ /g,'-') + "&utm_campaign=CC&lang=en";
	$("#facebook-utm").val(urlutmfacebook.replace(/ /g,''));
	$("#twitter-utm").val(urlutmtwitter.replace(/ /g,''));
	$("#linkedin-utm").val(urlutmlinkedin.replace(/ /g,''));	
}

/* Social Paid - LinkedIn */

$( "#field-6" ).change(function() { setLinkedInUTM(); });
$("#field-6").keyup(function() { setLinkedInUTM(); });
$( "#field-7" ).change(function() { setLinkedInUTM(); });
$("#field-7").keyup(function() { setLinkedInUTM(); });
$( "#field-8" ).change(function() { setLinkedInUTM(); });
$("#field-8").keyup(function() { setLinkedInUTM(); });

$( "#field-9" ).change(function() { setLinkedInUTM(); });
$( "#field-10" ).change(function() { setLinkedInUTM(); });
$( "#field-11" ).change(function() { setLinkedInUTM(); });

function setLinkedInUTM() {
	urlutmsponsored = $("#field-6").val() + "?utm_source=linkedin&utm_medium=sponsored&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	urlutmbanner = $("#field-6").val() + "?utm_source=linkedin&utm_medium=banner&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	urlutminmail = $("#field-6").val() + "?utm_source=linkedin&utm_medium=inmail&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	
	$("#sponsored-utm").val(urlutmsponsored.replace(/ /g,''));
	$("#banner-utm").val(urlutmbanner.replace(/ /g,''));
	$("#inmail-utm").val(urlutminmail.replace(/ /g,''));
}

$("#banner-1").click(function() {
	urlutmbanner = $("#field-6").val() + "?utm_source=linkedin&utm_medium=banner&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "-300x250".replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	$("#banner-utm").val(urlutmbanner.replace(/ /g,''));
});
$("#banner-2").click(function() {
	urlutmbanner = $("#field-6").val() + "?utm_source=linkedin&utm_medium=banner&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "-300x250-HTML".replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	$("#banner-utm").val(urlutmbanner.replace(/ /g,''));
});
$("#banner-3").click(function() {
	urlutmbanner = $("#field-6").val() + "?utm_source=linkedin&utm_medium=banner&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "-Spotlight".replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	$("#banner-utm").val(urlutmbanner.replace(/ /g,''));
});
$("#banner-4 ").click(function() {
	urlutmbanner = $("#field-6").val() + "?utm_source=linkedin&utm_medium=banner&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "-Follow".replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	$("#banner-utm").val(urlutmbanner.replace(/ /g,''));
});

$("#inmails-1").click(function() {
	urlutminmail = $("#field-6").val() + "?utm_source=linkedin&utm_medium=inmail&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "-Button-Banner".replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	$("#inmail-utm").val(urlutminmail.replace(/ /g,''));	
});
$("#inmails-2").click(function() {
	urlutminmail = $("#field-6").val() + "?utm_source=linkedin&utm_medium=inmail&utm_content=" + $("#field-7").val().replace(/ /g,'-').replace(/,/g,'-') + "-Body-Copy".replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-9").val() + "-" + $("#field-8").val() + "-" + $("#field-10").val() + $("#field-11").val() + "&lang=en";
	$("#inmail-utm").val(urlutminmail.replace(/ /g,''));	
});

/* Google Sponsored */

$( "#field-g6" ).change(function() { setGoogleUTM(); });
$("#field-g6").keyup(function() { setGoogleUTM(); });
$( "#field-g7" ).change(function() { setGoogleUTM(); });
$("#field-g7").keyup(function() { setGoogleUTM(); });
$( "#field-g8" ).change(function() { setGoogleUTM(); });
$("#field-g8").keyup(function() { setGoogleUTM(); });

$( "#field-g9" ).change(function() { setGoogleUTM(); });
$( "#field-g10" ).change(function() { setGoogleUTM(); });
$( "#field-g11" ).change(function() { setGoogleUTM(); });

function setGoogleUTM() {
	paidsearch = $("#field-g6").val() + "?utm_source=google&utm_medium=cpc&utm_content=" + $("#field-g7").val().replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-g9").val() + "-" + $("#field-g8").val() + "-" + $("#field-g10").val() + $("#field-g11").val() + "&lang=en";
	gdn = $("#field-g6").val() + "?utm_source=google&utm_medium=banner&utm_content=" + $("#field-g7").val().replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-g9").val() + "-" + $("#field-g8").val() + "-" + $("#field-g10").val() + $("#field-g11").val() + "&lang=en";
	gsp = $("#field-g6").val() + "?utm_source=google&utm_medium=banner&utm_content=" + $("#field-g7").val().replace(/ /g,'-').replace(/,/g,'-') + "-GSP&utm_campaign=" + $("#field-g9").val() + "-" + $("#field-g8").val() + "-" + $("#field-g10").val() + $("#field-g11").val() + "&lang=en";
	youtube = $("#field-g6").val() + "?utm_source=youtube&utm_medium=cpc&utm_content=" + $("#field-g7").val().replace(/ /g,'-').replace(/,/g,'-') + "&utm_campaign=" + $("#field-g9").val() + "-" + $("#field-g8").val() + "-" + $("#field-g10").val() + $("#field-g11").val() + "&lang=en";
	
	$("#paidsearch").val(paidsearch.replace(/ /g,''));
	$("#gdn").val(gdn.replace(/ /g,''));
	$("#gsp").val(gsp.replace(/ /g,''));
	$("#youtube").val(youtube.replace(/ /g,''));
}
