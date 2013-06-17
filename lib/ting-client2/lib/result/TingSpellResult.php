<?php
class TingSpellResult implements Iterator, Countable {
  /**
   * Raw data from webservice
   * @var OutputInterface
   */
  private $result;

  /**
   * Items in current result set.
   * Contains array of stdClass with ->name and ->count.
   * @var array
   */
  protected $items = array();

  // Iterator
  protected $_position;

  public function __construct(OutputInterface $result, TingSpellRequest $request) {
    $this->_position = 0;

    $this->result = $result;
    $this->process();
  }

  /**
   * Build items from raw data (json).
   */
  protected function process() {
    // Check for errors.
    $error = $this->result->getValue('spellResponse/error');
    if (!empty($error)) {
      throw new TingClientException($error);
    }

    $data = $this->result->getValue('spellResponse/term');

    if (!empty($data)) {
      foreach ($data as $term) {
        $item = new stdClass();
        $item->word = $term->getValue('suggestion');
        $item->weight = floatval(str_replace(',', '.', $term->getValue('weight')));
        $this->items[] = $item;
      }
    }

    $this->result = null;
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
    return count($this->items);
  }
}
