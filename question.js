jQuery(document).ready(function($) {
    webroot = "http://"+location.hostname+"/iParent1/";
	
	$('.grade').change(function(){
	    grd = $('.grade').attr('value');
	    $.ajax({
	        type : 'POST',
	        url : webroot + "get_subject",
	        data : {
				'grade' : grd
			},
			success : function(data){
			    output = $.parseJSON(data);
			    var html_data = "";
			    var html_data = "<option val=''>-- Select --</option>";
			    if(output){
			        $.each($.parseJSON(data), function(idx, obj) {
			            html_data += "<option value='"+ obj.id +"'>" + obj.subject_name + "</option>";	
                    });
                    $('.subject').html(html_data);
			    }
			    else{
			        alert('Somthing wrong, Please refresh page');
			    }
			}
			
        });
        
    });
    
    $('.common').click(function() {
        var grd = $(this).attr('id');
		//var cnt_val=$('#hdn'+grd).val();
        $.ajax({
	        type : 'POST',
	        url : webroot + "get_subject",
	        data : {
				'grade' : grd
			},
			success : function(data1){
			    //alert(data);
			    output1 = $.parseJSON(data1);
			    var html_data = "";
			    if(output1){
			        $.each($.parseJSON(data1), function(idx, obj) {
			            html_data += "<span id='div"+obj.id+"' target='targetDiv' style='float:left;'><p id='"+ obj.id +"' style='border:1px solid #000;margin:25px;width:225px;background-color: #efefef;height:104px !important;'><a href='forum/questions/"+grd+"/"+obj.id+"/'>" + obj.subject_name + "</a></p></span>";	
                    });
                    
                   $('.container_subjects').html(html_data);
				   $('#container_subjects').focus();					
                    window.scrollBy(0, 10000);
				 
			    }
			    else{
			        alert('Somthing wrong, Please refresh page');
			    }
			}
			
        });
    });
    
    $(".mobile").keypress(function (e) {
     //if the letter is not digit then display error and don't type anything
     if (e.which != 8 && e.which != 0 && (e.which < 48 || e.which > 57)) {
        //display error message
        $("#errmsg").html("<strong>Please enter proper mobile number.</strong>").show().fadeOut("slow");
               return false;
    }
   });
   $(".mobile").blur(function (e) {
        var mobile_len = $('.mobile').val();
        if (!($.trim(mobile_len).length == 10)) {
        //display error message
        $("#errmsg").html("<strong>Please enter 10 mobile number.</strong>").show().fadeOut("slow");
        $('.mobile').focus();
               return false;
    } 
   });
});
