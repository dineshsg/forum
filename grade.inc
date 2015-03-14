<?php
    function grades_settings_form($form_state)
    {
        $form['#attributes'] = array('enctype' => "multipart/form-data", 'class' => 'addgrades');

		$form['grades_name'] = array(
			'#type' => 'textfield',
			'#title' => t('Add Grades'),
			'#required' => TRUE,
			'#description' => "Please add grades.",
		);

		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit')
	   );
	   
	   return $form;
    }
    function grades_settings_form_submit($form, &$form_state)
    {
        global $user;
        $query = "INSERT INTO `forum_grades` (`grade_name`, `user_id`, `created_date`, `updated_date`) VALUES ('".$form_state['values']['grades_name']."', '".$user->uid."', (now()), (now()))";

        if(db_query($query)){
			drupal_set_message('New Grade Category Added Successfully');
		}else {
			drupal_set_message('Error while Adding Grade Category, Please try again.');
		}
    }
    function _list_grades()
    {
        if (!isset($output)) $output='';
        $form = drupal_get_form('grades_settings_form');
        $output .= drupal_render($form);
        $header = array(
		    array('data'=>'Grade', 'field'=>'grade_name', 'sort'=>'asc'),
		    array('data'=>'User Name', 'field'=>'name'),
		    array('data'=>'Operations', 'colspan' => 2),
    	);
        $result_grade = db_query('SELECT fg.grade_name,u.name,fg.id FROM forum_grades fg, users u WHERE fg.user_id = u.uid');
        
        foreach ($result_grade as $node) {
            $rows[] = array(
                'grade_name' => $node->grade_name,
                'name' => $node->name,
                'modify' => l('Modify', 'list/grade/modify/'.$node->id, array('html' => TRUE)),
                'delete' => l('Delete', 'list/grade/delete/'.$node->id, array('html' => TRUE)),
            );
        }
        if(isset($rows))
        {
            $output .= theme('table', array('header' => $header,'rows' => $rows ));
        }
        else
        {
            $output .= "<br><h3>No Data</h3><br>";
        }
        return $output;
    }
    function grade_modify_form($form, &$form_state, $operation = 'add', $folk_id = NULL)
    {
        $grade_id = arg(3);
        $result_grade = db_query('SELECT fg.grade_name,u.name,fg.id FROM forum_grades fg, users u WHERE fg.id = :nid AND fg.user_id = u.uid',array(':nid' => $grade_id)) ->fetchAll();
        $result_grade1 = $result_grade[0]->grade_name;
        $form['grades_name_update'] = array(
			    '#type' => 'textfield',
			    '#title' => t('Add Grades'),
			    '#required' => TRUE,
			    '#description' => "Please add grades.",
			    '#value' => $result_grade1,
	    ); 
	    $form['submit'] = array(
	            '#type' => 'submit',
			    '#value' => t('Submit')
	    );
        return $form;
    }
    function grade_modify_form_submit($form, &$form_state)
    {
        global $user;
        $cur_time = $mydate = date('Y-m-d', strtotime(date('d/m/Y')));
        $query = "UPDATE `forum_grades` SET `grade_name`='".$_POST['grades_name_update']."', `user_id` = '".$user->uid."', `updated_date` = '".$cur_time."' WHERE id = '".arg(3)."'";
        if(db_query($query)){
            drupal_set_message('New Grade Category Added Successfully');
	    }else {
	        drupal_set_message('Error while Adding Grade Category, Please try again.');
	    }
	    $form_state['redirect'] = 'admin/list_grades';  
    }
    function grade_delete_confirm($form, &$form_state)
    {
        $grade_id = arg(3);
        $result_grade_del = db_query('SELECT grade_name FROM forum_grades WHERE id = :nid',array(':nid' => $grade_id)) ->fetchAll();
        if($result_grade_del)
        {
            $result_grade_del1 = $result_grade_del['0']->grade_name;
            return confirm_form($form, t('Are you sure you want to delete %grade', array('%grade' => $result_grade_del['0']->grade_name)),'list/grade/delete/'.arg(3),t('This action cannot be undone'),t('Delete'),t('Cancel'));
        }
        else
        {
            drupal_goto('admin/list_grades');
        }
    }
    function grade_delete_confirm_submit($form, $form_state)
    {
        if($form_state['values']['confirm'])
        {
            $grade_id = arg(3);
            $result = "DELETE FROM forum_grades WHERE id='".$grade_id."'";
            if(db_query($result))
            {
                drupal_set_message('Grade has been sucessfully deleted');
            }
            else
            {
                drupal_set_message('Error while Deleting Grade Category, Please try again.');
            }
        }
        drupal_goto('admin/list_grades');  
    }
?>
