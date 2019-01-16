// Check if status set to 200 and show modal and add video.

$(document).ready(function() {
	if(getUrlParameter('status') == "200") {
		
		$(".modal-background").css("display", "block");
		$(".modal-background").css("opacity", "1");
		
		$(".modal-window").css("display", "block");
		$(".modal-window").css("opacity", "1");

	}

	else if(getUrlParameter('status') == "300") {
		alert("مهلا! يبدو أن الصورة التي حملتها ليست من النوع الصور المقبول. يرجى استخدام ملف من نوع .png .jpg .jpeg أو .gif وإعادة المحاولة.");
	}

	else if(getUrlParameter('status') == "400") {
		alert("مهلا! يبدو أن حجم الصورة التي حملتها يتجاوز الحد الأقصى للحجم المسموح به. يرجى استخدام صورة حجمها أقل من 5 ميغابايت وإعادة المحاولة.");
	}

	/* Remove get parameters so users can copy the URL and share */
	var url = refineUrl();
	history.pushState(null, null, url);
});

// On clicking the close button or modal, hide the modal and remove video.
$(document).ready(function() {
	$(".close-button").click(function() {
		closeModal();
	});
});

/* Close Modal */

function closeModal() {
		$(".modal-window").css("opacity", "0");
		$(".modal-background").css("opacity", "0");
		
		setTimeout(function() {
			$(".modal-window").css("display", "none");
			$(".modal-background").css("display", "none");
    	}, 200);
}

/* Get URL parameter function */
var getUrlParameter = function getUrlParameter(sParam) {
    var sPageURL = decodeURIComponent(window.location.search.substring(1)),
        sURLVariables = sPageURL.split('&'),
        sParameterName,
        i;

    for (i = 0; i < sURLVariables.length; i++) {
        sParameterName = sURLVariables[i].split('=');

        if (sParameterName[0] === sParam) {
            return sParameterName[1] === undefined ? true : sParameterName[1];
        }
    }
};

/* Refine URL - Remove get parameters */
var refineUrl = function refineUrl()
{
    var url = window.location.href;
    var value = url.substring(url.lastIndexOf('/') + 1);
    value  = value.split("?")[0];   
    return value;     
}