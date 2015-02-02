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
        /*$form['file'] = array(
            '#type' => 'file',
            '#name' => 'files[]',
            '#title' => t('Attach Multiple Files'),
            '#description' => t('You are allow to upload jpg, jpeg, png and gif, 10MB Max Size'),
            '#attributes' => array('multiple' => 'multiple'),
        );*/
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
	
	/*function ask_a_question_form_validate($form, &$form_state)
	{
        $num_files = count($_FILES['files']['name']);
        $directory = 'var/www/html/iParent1/sites/default/attachment/'.$form_state['values']['grade'].'/'.$form_state['values']['subject'];
        if(file_exists($directory) && is_dir($directory)) 
        { 
        }
        else
        {
            if(mkdir($directory , 0777, true)) 
            { 
               echo "Directory has been created successfully..."; 
            } 
            else 
            { 
               echo "Failed to create directory..."; 

            }
        }
        for ($i = 0; $i < $num_files; $i++) {
            $file = file_save_upload($i, array(
                'file_validate_is_image' => array(),
                'file_validate_extensions' => array('png gif jpg jpeg'),
            ));
            if ($file) {
                  //if ($file = file_move($file, '/sites/default/attachment/'.$form_state['values']['grade'].'/'.$form_state['values']['subject'])) {
                  if(move_uploaded_file($_FILES['files']['tmp_name'][$i], $_SERVER['DOCUMENT_ROOT'].'/iParent1/sites/default/attachment/'.$form_state['values']['grade'].'/'.$form_state['values']['subject'])){
                        //$form_state['values']['file'][$i] = $file;
                  }
                  else {
                        form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
                  }
            }
            else {
                  form_set_error('file', t('No file was uploaded.'));
            }
      }
	}*/
	
	function ask_a_question_form_submit($form, &$form_state)
	{
	    global $user;
	    global $base_root;
        $current_url = $base_root.request_uri();
        
	    $pre_text = escape($form_state['values']['question_desc']);
        $query = "INSERT INTO `forum_questions` (`question`, `user_id` ,`grade_id`, `subject_id`, `status`,`created`) VALUES ('".$form_state['values']['question_desc']."', '".$user->uid."',  '".$form_state['values']['grade']."', '".$form_state['values']['subject']."','0',CURRENT_TIMESTAMP)";
        
        if(db_query($query)){
			drupal_set_message('New Question has been posted Successfully');
		}else {
			drupal_set_message('Error while posting a question , Please try again.');
		}
		
		$to = "dineshsgaddi@gmail.com"; // to Teacher e-mail address
        $from = "noreply@iparent.com"; // from site email address
        $language = user_preferred_language($user);
 
        $subject = "A Question has been posted on the forum"; // subject of e-mail
        $body = array('0'=>t("Following question has been posted on the forum <br> URL:'".$current_url."'")); //it might be any variable from the form eg. $form_state['values']['your_field']
 
        //params is the array passed to hook_mail function
        $params = array(
            'subject' => $subject,
            'body' => $body,
            );
         
        drupal_mail('forum', 'askaquestion', $to, $language, $params, $from);
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
	       $output .= "<td><span><a href='/iParent1/forum/question-detail/".$rows->id."'>".substr($norm_text, 0, 100)."</a></span><div class='algn-right small-font'>".$date_ago."</div></td>";
	       $output .= "<td class='user-name algn-right'>by <span>".$username."</span></td></tr>";
       }
       $output .= '</table>';
       }
       else
	   {
	    $output .= "No questions have been posted.<a href='#'>Go Back to Forum</a>";
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
  	    $output .= "<div style='float:left;'>
  	                <div>
  	                    <div class='divider color'>Grade</div>
  	                    <div class='divider'>".$rows->grade_name."</div>
  	                </div>";
  	    $output .= "<div>
  	                <div class='divider color'>Subject</div>
  	                <div class='divider'>".$rows->subject_name."</div>
  	                </div>
  	                </div>";
        $date_ago = ago($rows->created);
	    $user=user_load($rows->user_id);
        $username=$user->name;
        $output .= "<div class='space'>
                        <div>
                        <span>Posted By:</span><strong><span>".$username."</span></strong>
                        </div>
                        <div>
                        <span>Date - </span><strong><span>".date('F jS Y', strtotime($rows->created))."</span></strong>
                        </div>";
        $output .= "<div>
                        <div>Question:</div>
                        <div style='text-align:center;'><p style='text-indent: auto;'>".$rows->question."</p></div>
                        <!--<div style='text-align:right;'></div>-->
                    </div>
                    </div>";
        }
        $output .= "</div>";
        
        $form_question = drupal_get_form("question_reply_form", $question_id);
        
        if($question_reply->rowCount() > 0)
	    {
            while($reply = $question_reply->fetchObject())
            {
                $output .= "<div class='space'>
                            <div>
                            <span>Replied By:</span><strong><span>".$username."</span></strong>
                            </div>
                            <div>
                            <span>Date - </span><strong><span>".date('F jS Y', strtotime($reply->created))."</span></strong>
                            </div>";
                $output .= "<div id='form_{$reply->id}' class='form-wrapper'>";
                $output .= "<div>".nl2br($reply->reply_message)."</div>";
                $output .= "</div>";
                $output .= "</div><br>";
            }
        }
        $output .= "<div class='space'>";
        $output .= drupal_render($form_question);
        $output .= "</div>";
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
		
		$get_teacher = db_query("SELECT u.name,u.mail FROM users u WHERE ques.id = '".$form_state["values"]["hidden"]."' AND ques.grade_id =  ");
		
		$to = "dineshsgaddi@gmail.com"; // to student e-mail address
        $from = "noreply@iparent.com"; // from site email address
        $language = user_preferred_language($user);
 
        $subject = "A reply has been posted on your question"; // subject of e-mail
        $body = array('0'=>t("Following question has been replied on the forum <br> URL:'".$current_url."'")); //it might be any variable from the form eg. $form_state['values']['your_field']
 
        //params is the array passed to hook_mail function
        $params = array(
            'subject' => $subject,
            'body' => $body,
            );
         
        drupal_mail('forum', 'replyaquestion', $to, $language, $params, $from);
    }
    
    function ago($timestamp){
        $difference = time() - strtotime($timestamp);
        $text = format_interval($difference, 1) . " ago";
        return $text;    
    }
    function escape($value) {
        /*$return = '';
        for($i = 0; $i < strlen($value); ++$i) {
            $char = $value[$i];
            $ord = ord($char);
            if($char !== "'" && $char !== "\"" && $char !== '\\' && $ord >= 32 && $ord <= 126)
                $return .= $char;
            else
                $return .= '\\x' . dechex($ord);
        }*/
        return htmlspecialchars($value);
    }
?>
