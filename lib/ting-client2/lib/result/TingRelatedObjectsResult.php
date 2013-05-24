<?php
class TingRelatedObjectsResult implements Iterator, Countable {
  /**
   * Raw data from webservice
   * @var OutputInterface
   */
  private $result;

  /**
   * Objects on this page.
   * @var int
   */
  protected $collectionCount;

  /**
   * Items in current result set.
   * Contains TingObject or TingEntity.
   * @var array
   */
  protected $items = array();

  // Iterator
  protected $_position;

  public function __construct(OutputInterface $result, TingRelatedObjectsRequest $request) {
    $this->_position = 0;
    $this->result = $result;
    $this->process();
  }

  /**
   * Build items from raw data (json).
   */
  protected function process() {
    // Check for errors.
    $error = $this->result->getValue('searchResponse/error');
    if (!empty($error)) {
      throw new TingClientException($error);
    }

    list($data) = $this->result->getValue('searchResponse/result/searchResult');
    $this->collectionCount = $data->getValue('collection/numberOfObjects');

    $items = $data->getValue('collection/object');

    if (!empty($items)) {
      foreach ($items as $item) {
        $this->items[] = new TingObject($item);
      }
    }

    $this->result = null;
  }

  public function setItems($items) {
    $this->items = array();
    foreach ($items as $item) {
      $this->items[] = $item;
    }
    $this->rewind();
  }

  /*
   * (non-PHPdoc) @see Iterator::current()
   */
  public function current() {
    return $this->items[$this->_position];
  }

  /*
   * (non-PHPdoc) @see Iterator::next()
   */
  public function next() {
    ++$this->_position;
  }

  /*
   * (non-PHPdoc) @see Iterator::key()
   */
  public function key() {
    return $this->_position;
  }

  /*
   * (non-PHPdoc) @see Iterator::valid()
   */
  public function valid() {
    return isset($this->items[$this->_position]);
  }

  /*
   * (non-PHPdoc) @see Iterator::rewind()
   */
  public function rewind() {
    $this->_position = 0;
  }

  /*
   * (non-PHPdoc) @see Countable::count()
   */
  public function count() {
    return $this->collectionCount;
  }
}
