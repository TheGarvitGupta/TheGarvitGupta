$(document).ready(function() {

	$.getJSON('https://ipinfo.io', function(data){

		if (["AE", "SA", "QA", "OM", "BH", "KW", "EG", "LB"].indexOf(data.country) >= 0) {
	    	$(".select-box").val(data.country);
			
	    	if (data.country == "AE") {
	    		$("[name='phone']").val("+971 ");	
	    	}
			else if (data.country == "SA") {
	    		$("[name='phone']").val("+966 ");	
	    	}
	    	else if (data.country == "QA") {
	    		$("[name='phone']").val("+974 ");	
	    	}
	    	else if (data.country == "OM") {
	    		$("[name='phone']").val("+968 ");	
	    	}
	    	else if (data.country == "BH") {
	    		$("[name='phone']").val("+973 ");	
	    	}
	    	else if (data.country == "KW") {
	    		$("[name='phone']").val("+965 ");	
	    	}
	    	else if (data.country == "EG") {
	    		$("[name='phone']").val("+20 ");	
	    	}
	    	else if (data.country == "LB") {
	    		$("[name='phone']").val("+961 ");	
	    	}
		}

		if (data.country == "AE") {

			/* Link to Products Page */

			$(".shop-button-1").click(function() {
				window.location.href = "http://google.com";
			});
			$(".shop-button-2").click(function() {
				window.location.href = "http://google.com";
			});		

			/* Store Links Set to UAE */

			$(".store").click(function() {
				window.location.href = "http://www.babyshopstores.com/ae/en/storelocator";
			});
		}
		else {
			$(".shop-button-1").click(function() {
				window.location.href = "https://www2.babyshopstores.com/sa/en";
			});
			$(".shop-button-2").click(function() {
				window.location.href = "https://www2.babyshopstores.com/sa/en";
			});
			$(".shop-button-1").html("Store Locations");
			$(".shop-button-2").html("Store Locations");

			/* Store Link Set to GCC */

			$(".store").click(function() {
				window.location.href = "https://www2.babyshopstores.com/sa/en";
			});

			/* Hide Shop Cars 3 Merchadize Link */
			$(".shops").css("display", "none");
		}
	})

});
