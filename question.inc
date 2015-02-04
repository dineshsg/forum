<?php

    /*------------------------------------
        Form for student asking a question
    --------------------------------------*/
	function ask_a_question_form($form_state)
	{
	    $result_grade = db_query('SELECT id,grade_name FROM forum_grades') ->fetchAll();
        $grd_nm = $result_grade[0]->grade_name;
        $grd_id = $result_grade[0]->id;
        $option[''] = '----Select----';        
        foreach ($result_grade as $node) {
            $option[$node->id] = $node->grade_name;
        }
        $form['grade'] = array(
            '#title' => t('Grade'),
            '#type' => 'select',
            '#options' => $option,
            '#attributes' => array('class' => array('grade')),
            '#validated' => TRUE,
        );
        $form['subject'] = array(
            '#title' => t('Subject'),
            '#type' => 'select',
            '#options' => array(''=> '-- Select --'),
            '#attributes' => array('class' => array('subject')),
            '#validated' => TRUE
        );
        $form['file'] = array(
            '#type' => 'file',
            '#name' => 'files[]',
            '#title' => t('Attach Multiple Files'),
            '#description' => t('You are allow to upload jpg, jpeg, png and gif, 10MB Max Size'),
            '#attributes' => array('multiple' => 'multiple'),
        );
        $form['question_desc'] = array(
          '#title' => t('Ask your Question'),
          '#type' => 'textarea',
        );
        $form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit'),
	    );
        return $form;
	}
	
	function ask_a_question_form_submit($form, &$form_state)
	{
	    global $user;
	    $pre_text = escape($form_state['values']['question_desc']);
	    $query = "INSERT INTO `forum_questions` (`question`, `user_id` ,`grade_id`, `subject_id`, `status`,`created`) VALUES ('".$pre_text."', '".$user->uid."',  '".$form_state['values']['grade']."', '".$form_state['values']['subject']."','0',CURRENT_TIMESTAMP)";
	    if(db_query($query)){
			drupal_set_message('New Question has been posted Successfully');
		}else {
			drupal_set_message('Error while posting a question , Please try again.');
		}
	    $last = db_query("SELECT MAX(id) as last FROM forum_questions");
	    $last = $last->fetchObject();
	    $last = $last->last;
	    /*---------------------------------------------------------------------
	     Code for attaching multiple files along with the question on the forum
	    ----------------------------------------------------------------------*/
	    $num_files = count($_FILES['files']['name']);
        $file_name = $_FILES['files']['name'];
        $directory = 'sites/default/attachment/'.$form_state['values']['grade'].'/'.$form_state['values']['subject'].'/'.$last;
        $str = variable_get('exten_allowed', '');
        $allowedExts = explode(",",$str);
        
        if(!file_exists($directory) && !is_dir($directory)) 
        {
            mkdir($directory , 0777, true);
            chmod($directory,0777);
        }

	    foreach($_FILES['files']['name'] as $key => $name)
        {
            $tmp_name = $_FILES['files']['tmp_name'][$key];
            $name = $_FILES['files']['name'][$key];
            $tmp = explode(".", $name);
            $extension=end($tmp);
            $name="Attachment-".$key.".".$extension;
            //if (($_FILES['files']['size'][$key] < 20000) && in_array($extension, $allowedExts))
            if (in_array($extension, $allowedExts))
            {
                $moved = move_uploaded_file($tmp_name, $directory.'/'.$name);
            }
            else
            {
                drupal_set_message(t('Some file cannot be uploaded on the forum. Please contact the administrator of this site.'), 'error');
            }
            $path_name = $directory.'/'.$name;
            $query = db_query("INSERT INTO `forum_question_attach` (`question_id`, `file_name` ,`file_type`) VALUES ('".$last."', '".$path_name."','".$_FILES['files']['type'][$key]."')");
        }
        
	    
        
        //$query = "INSERT INTO `forum_question_attach` (`question_id`, `file_name` ,`file_type`) VALUES ('".$pre_text."', '".$path_name."',  '".$form_state['values']['grade']."', '".$form_state['values']['subject']."')";
        
	}
	
	function forum()
	{
	    //return 1;
	}
	
	/*------------------------------------------
	    Listing all the questions on the forum
	-------------------------------------------*/
	function _forum_question()
	{   
	   global $base_path;
	   $grades = arg(2);
	   $subjects = arg(3);
	   $output='';
	   if (!($grades)||!($subjects))
	   {
           $question_query = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.grade_id = grds.id and ques.subject_id = sub.id and ques.status = 1');	   
	   }
	   else
	   {
	       $question_query = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.grade_id = '.$grades.' and ques.subject_id = '.$subjects.' and grds.id = '.$grades.' and sub.id = '.$subjects.' and ques.status = 1');
	   }
	   if($question_query->rowCount() > 0)
	   {
       $output .= "<table class='table table-striped'>";
	   while($rows = $question_query->fetchObject()){
	       $output .= "<tr><td class='img'><span class='question-img'></span></td>";
	       $date_ago = ago($rows->created);
	       $user=user_load($rows->user_id);
           $username=$user->name;
           $norm_text = escape($rows->question);
	       $output .= "<td><span><a href='/iParent/forum/question-detail/".$rows->id."'>".substr($norm_text, 0, 100)."</a></span><div class='algn-right small-font'>".$date_ago."</div></td>";
	       $output .= "<td class='user-name algn-right'>by <span>".$username."</span></td></tr>";
       }
       $output .= '</table>';
       }
       else
	   {
	    $output .= "<div class='sub-title clear'>No questions have been posted.<a href='#'>Go Back to Forum</a></div>";
	   }

	   return $output;
	}

    /*------------------------------------------------------
        This page is to view single question and there reply
    -------------------------------------------------------*/
    
    function _question()
    {
        $question_id = arg(2);
        $output = '';
        $question_detail = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.grade_id = grds.id and ques.subject_id = sub.id and ques.id = '.$question_id.'');
        $question_reply = db_query('SELECT qr.reply_message, qr.id, qr.created FROM forum_question_reply as qr WHERE qr.question_id = '.$question_id.'');
        $output .= "<div>";
  	    while($rows = $question_detail->fetchObject()){
  	   /* $output .= "<div class='col-12'><div style='float:left;'>
  	                <div>
  	                    <div class='divider color'>Grade</div>
  	                    <div class='divider'>".$rows->grade_name."</div>
  	                </div>";
  	    $output .= "<div>
  	                <div class='divider color'>Subject</div>
  	                <div class='divider'>".$rows->subject_name."</div>
  	                </div>
  	                </div>";*/
		 $output .= "<div class='col-12'><div class='img_bg border_blue'><div class='space'>
  	                <div class='font-100 custom_font2 content_down '>
  	                    Grade :<label class='font_black font-300'> ".$rows->grade_name." </label> &nbsp; &nbsp;               
  	                    Topic :<label class='font_black font-300'> ".$rows->subject_name." </label> 
  	                </div></div>";
  	    
        $date_ago = ago($rows->created);
	    $user=user_load($rows->user_id);
        $username=$user->name;
        $output .= "<div class='space content-font'>
                        <div>
                        	<span>Posted By:</span><strong><span>".$username."</span></strong>
                        </div>
                        <div>
                        	<span>Date - </span><strong><span>".date('F jS Y', strtotime($rows->created))."</span></strong>
                    </div>";
        $output .= "<div class='content-font'>
                        <div>Question:</div>
                        <h5 class='font-300 font-black'><p style='text-indent: auto;'><strong>".$rows->question."</strong></p></h5></div></div>";
        }
    //    $output .= "</div>";
       
        $form_question = drupal_get_form("question_reply_form", $question_id);
        
        if($question_reply->rowCount() > 0)
	    {
            while($reply = $question_reply->fetchObject())
            {
                $output .= "<div class='space content-font'>
                            <div>
                            <span>Replied By:</span><strong><span>".$username."</span></strong>
                            </div>
                            <div>
                            <span>Date - </span><strong><span>".date('F jS Y', strtotime($reply->created))."</span></strong>
                            </div>";
                $output .= "<div id='form_{$reply->id}' class='form-wrapper'>";
                $output .= "<div>Answer:</div><h5 class='font-300 font-black'><strong>".$reply->reply_message."</strong></h5>";
                $output .= "</div>";
                $output .= "</div><br>";
            }
        }
        $output .= "<div class='space'>";
        $output .= drupal_render($form_question);
        $output .= "</div></div></div>";
        return $output;
    }
    
    /*
        This form is for replying the questions on forum
    */
    
    function question_reply_form($form, &$form_state, $question_id) {
        $form = array();
        $form["#submit"] = array(
            'question_reply_form_submit',
        );

        $form["content"] = array(
            '#type' => 'textarea',
            '#wysiwyg' => True
        );
        $form["hidden"] = array(
            '#type' => 'hidden',
            '#value' => $question_id
        );
        $form["submit"] = array(
            '#type' => 'submit',
            '#value' => "Send Mail"
        );
        return $form;
    }
    
    function question_reply_form_submit($form, &$form_state)
    {
        global $user;
        $query = "INSERT INTO `forum_question_reply` (`question_id`, `user_id` ,`reply_message`, `created`) VALUES ('".$form_state["values"]["hidden"]."', '".$user->uid."',  '".$form_state["values"]["content"]."',CURRENT_TIMESTAMP)";        
        if(db_query($query)){
			drupal_set_message('Comment has been posted Successfully');
		}else {
			drupal_set_message('Error while posting a commment , Please try again.');
		}
    }
    
    function ago($timestamp){
        $difference = time() - strtotime($timestamp);
        $text = format_interval($difference, 1) . " ago";
        return $text;    
    }
    function escape($value) {
    $return = '';
    for($i = 0; $i < strlen($value); ++$i) {
        $char = $value[$i];
        $ord = ord($char);
        if($char !== "'" && $char !== "\"" && $char !== '\\' && $ord >= 32 && $ord <= 126)
            $return .= $char;
        else
            $return .= '\\x' . dechex($ord);
    }
    return $return;
    }
?>
