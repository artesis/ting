<?php

class TingObjectResult {

  /**
   * Raw data from webservice
   * @var OutputInterface
   */
  private $result;

  /**
   * Item in current result set.
   * Contains TingObject or TingEntity.
   * @var array
   */
  protected $item;

  public function __construct(OutputInterface $result, TingObjectRequest $request) {
    $this->result = $result;
    $this->process();
  }

  /**
   * Build items from raw data (json).
   */
  protected function process() {
    $data = $this->result->getValue('searchResponse/result');
    $items = $data->getValue('searchResult');
    $this->item = new TingObject($items[0]);
    $this->result = null;
  }

  public function getObject() {
    return $this->item;
  }

}
