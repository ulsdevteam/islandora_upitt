<?php
/**
	* @file
	* Islandora-upitt-view display template.
	*
	* Variables available:
	* - $upitt_date: The defined date corresponding to the Solr field.
	* - $upitt_creator: The defined creator corresponding to the Solr field.
	* - $upitt_lg_thumb: The defined large thumbnail image corresponding to the Solr field.
	*
	* @see template_preprocess_islandora-upitt-view
	*/
?>

<div id="two-col-left-main">
	<?php
		print '<p class="subtitle-first">' . $upitt_date . '</p>';
		print '<p class="subtitle">' . $upitt_creator . '</p>';
		echo '<br />';
	?>
	<div id="content">
    <div id="object-pg-left-col">
      <?php
        print $upitt_lg_thumb;
      ?>
      <div class="widget">
      <?php
      $block = block_load('user', 'new');
      $render_array = _block_get_renderable_array(_block_render_blocks(array($block)));
      $output = render($render_array);
      print $output;

      ?>
     	</div><!-- /end widget -->
    </div><!-- /end ojbect-pg-left-col -->

    <div id="object-pg-right-col">
      <?php print $metadata; ?>
    </div><!-- /end ojbect-pg-right-col -->
  </div><!-- /end content -->
</div><!-- /end two-col-left-main -->